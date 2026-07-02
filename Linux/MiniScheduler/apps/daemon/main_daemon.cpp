//==================================================================================================================
// main_daemon.cpp
// Note: Main daemon process for the MiniScheduler project.
//       Running in background, initialize the IPC(Unix domain socket).
// Flow: Create socket -> Listen client -> Connect(accept) client -> Read from client -> Close socket
//       Shared memory + multi worker processes.
//==================================================================================================================

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "../../include/ipc/unix_socket.h"
#include "../../include/ipc/def.h"
#include "../../include/config/config.h"
#include "../../include/config/config_loader.h"
#include "../../include/job/job.h"


extern char g_log_path[1024];    // Global variable to hold the log path, defined in unix_socket.cpp

// volatile is used to prevent compiler optimizations so that main process can access the variable stored in memory instead of register cache.
// sig_atomic_t is used to ensure that the variable is accessed atomically, which is important for signal handlers to avoid race conditions.
static volatile sig_atomic_t g_running = 1;     // Flag to control the main loop.

// Global pointer to the shared memory scheduler (accessible by daemon and workers after fork).
static shared_scheduler_t *g_scheduler = NULL;

static job_t* find_job_in_history_jobs(size_t job_id);
static int fork_workers(int num_workers);
static void worker_process_main();


/// @brief  Find a job in the history jobs array by its ID.
/// @param  job_id 
/// @return 
/// @note
static job_t* find_job_in_history_jobs(size_t job_id)
{
    for( size_t i = 0; i < g_scheduler->history_jobs_size; i++ ) {
        if(g_scheduler->history_jobs[i].id == job_id) {
            return &g_scheduler->history_jobs[i];
        }
    }
    
    return NULL;
}


/// @brief  Fork worker processes.
/// @param  num_workers  Number of workers to fork.
/// @return 0 : Success
///         -1: Error
/// @note   Child processes run worker_process_main() and never return.
static int fork_workers(int num_workers)
{
    if( num_workers > MINISCHED_MAX_WORKERS ) {
        log_error("[%s][%s:%d] Number of workers exceeds maximum limit (%d), using %d instead.\n", __FILE__,__func__,__LINE__, num_workers, MINISCHED_MAX_WORKERS);
        num_workers = MINISCHED_MAX_WORKERS;
    }

    for( int i = 0; i < num_workers; i++ ) {
        pid_t pid = fork();
        if( pid < 0 ) {
            log_error("[%s][%s:%d] fork() failed for worker %d\n", __FILE__,__func__,__LINE__, i);
            return -1;
        } else if( pid == 0 ) {
            // Child process: run worker loop.
            worker_process_main();
            _exit(0);
        } else {
            // Parent process: record the child PID.
            g_scheduler->worker_pids[i] = pid;
            g_scheduler->num_workers = i + 1;
            log_out("[%s][%s:%d] Forked worker %d [PID: %d]\n", __FILE__,__func__,__LINE__, i, pid);
        }
    }

    return 0;
}

/// @brief  Worker process main function.
/// @note   Pops jobs from the shared memory queue and executes them.
///         If mutex owner died, make it consistent (robust mutex).
static void worker_process_main()
{
    log_out("[%s][%s:%d] Worker process [PID: %d] is running.\n", __FILE__,__func__,__LINE__, getpid());

    while(1) {
        pthread_mutex_lock(&g_scheduler->queue.mutex);

        // Wait until there is a job in the queue or shutdown is requested.
        while( job_queue_is_empty(&g_scheduler->queue) && !g_scheduler->shutting_down ) {
            pthread_cond_wait(&g_scheduler->queue.cond, &g_scheduler->queue.mutex);
        }

        // If the queue is empty and shutting down, exit the loop.
        if( job_queue_is_empty(&g_scheduler->queue) && g_scheduler->shutting_down ) {
            log_out("[%s][%s:%d] Worker [PID: %d] shutting down.\n", __FILE__,__func__,__LINE__, getpid());
            pthread_mutex_unlock(&g_scheduler->queue.mutex);
            break;
        }

        job_t current_job;

        // Pop a job from the queue.
        if( job_queue_pop(&g_scheduler->queue, &current_job) == -1 ) {
            log_error("[%s][%s:%d] Worker [PID: %d] failed to pop job\n", __FILE__,__func__,__LINE__, getpid());
            pthread_mutex_unlock(&g_scheduler->queue.mutex);
            continue;
        }

        // Update job state in shared memory.
        current_job.state = JOB_RUNNING;
        get_local_time_string(current_job.started_time, sizeof(current_job.started_time), time(NULL), LOCAL_TIME_FORMAT_STRING);
        current_job.pid = getpid();

        job_t *history_job = find_job_in_history_jobs(current_job.id);
        if( history_job != NULL ) {
            history_job->state = current_job.state;
            strncpy(history_job->started_time, current_job.started_time, sizeof(history_job->started_time) - 1);
            history_job->pid = current_job.pid;
        }

        pthread_mutex_unlock(&g_scheduler->queue.mutex);

        // Execute the job command using system().
        // Redirect the output to a command log file.
        char sys_cmd[MINISCHED_MAX_CMD_SIZE+MINISCHED_MAX_STRING_SIZE] = {0};
        char cmd_log_path[1024] = {0};
        int log_flag = 0;
        if(g_log_path[0] != '\0') {
            // Insert "_cmd" before extension.
            const char *last_dot = strrchr(g_log_path, '.');
            if( last_dot != NULL ) {
                int dir_len = last_dot - g_log_path;  // not include the '.'.
                snprintf(cmd_log_path, sizeof(cmd_log_path), "%.*s_cmd%s", dir_len, g_log_path, last_dot);
            }

            int fd = open(cmd_log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if( fd >= 0 ) {
                log_flag = 1;
                dprintf(fd, "\nExecuting job ID[%d] by worker [PID: %d]: %s\n", current_job.id, getpid(), current_job.command);
                snprintf(sys_cmd, sizeof(sys_cmd), "%s >> %s 2>&1", current_job.command, cmd_log_path);
                close(fd);
            }
        }

        if(!log_flag) {
            snprintf(sys_cmd, sizeof(sys_cmd), "%s", current_job.command);
        }

        int return_code = system(sys_cmd);

        // Update job state after execution.
        pthread_mutex_lock(&g_scheduler->queue.mutex);

        current_job.exit_code = return_code;
        current_job.state = (return_code == 0) ? JOB_COMPLETED : JOB_FAILED;
        get_local_time_string(current_job.finished_time, sizeof(current_job.finished_time), time(NULL), LOCAL_TIME_FORMAT_STRING);

        if( history_job != NULL ) {
            history_job->state = current_job.state;
            history_job->exit_code = current_job.exit_code;
            strncpy(history_job->finished_time, current_job.finished_time, sizeof(history_job->finished_time) - 1);
        }

        pthread_mutex_unlock(&g_scheduler->queue.mutex);

        log_out("[%s][%s:%d] Worker [PID: %d] finished job ID[%d], exit_code=%d\n",
                __FILE__,__func__,__LINE__, getpid(), current_job.id, return_code);
    }

    log_out("[%s][%s:%d] Worker process [PID: %d] exiting.\n", __FILE__,__func__,__LINE__, getpid());
    _exit(0);
}

//-------------------------------------------
// Note: Handle termination signals to allow graceful shutdown of the daemon.
//
static void handle_signal(int signo) {
    if( signo == SIGINT || signo == SIGTERM ) {
        g_running = 0;
    }
}

// DAEMON MAIN.
int main()
{
    printf("[%s][%s:%d] MiniScheduler daemon starting...\n", __FILE__,__func__,__LINE__);
    MainDaemon *c_MainDaemon = MainDaemon::newInstance();

    // Get the absolute log path.
    if( log_path_init() != 0 ) {
        printf("[%s][%s:%d] Failed to initialize log path\n",__FILE__,__func__,__LINE__);
        return 1;
    }

    // Load config data before daemonize().
    log_out("[%s][%s:%d] Loading config file.\n", __FILE__,__func__,__LINE__);
    minisched_config_t config;
    config_load(MINISCHED_CONFIG_FILEPATH, &config);

    // Daemonize the process to run in the background.
    log_out("[%s][%s:%d] Starting the MiniScheduler daemon process...\n", __FILE__,__func__,__LINE__);
    if( MainDaemon::daemonize(MINISCHED_PID_FILEPATH) != 0 ) {
        log_error("[%s][%s:%d] Daemonize failed.\n",__FILE__,__func__,__LINE__);
        return 1;
    }

    // signal(SIGINT, handle_signal);
    // signal(SIGTERM, handle_signal);

    // Set up signal handlers for graceful shutdown.
    // Use sigaction() without SA_RESTART so that blocking syscalls (accept) return EINTR on signal.
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;                    // No SA_RESTART: accept() will return -1 with errno = EINTR.
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Create shared memory for the scheduler.
    g_scheduler = shm_scheduler_create();
    if( g_scheduler == NULL ) {
        log_error("[%s][%s:%d] Failed to create shared scheduler\n",__FILE__,__func__,__LINE__);
        return 1;
    }

    // Fork worker processes.
    log_out("[%s][%s:%d] Forking %d worker processes...\n", __FILE__,__func__,__LINE__, config.num_workers);
    if( fork_workers(config.num_workers) != 0 ) {
        log_error("[%s][%s:%d] Failed to fork worker processes\n",__FILE__,__func__,__LINE__);
        shm_scheduler_destroy(g_scheduler);
        return 1;
    }

    // Create and start listening on the Unix domain socket.
    log_out("[%s][%s:%d] Starting IPC server on socket: %s\n", __FILE__,__func__,__LINE__, config.socket_path);
    int server_fd = c_MainDaemon->ipc_server_listen(config.socket_path);
    if( server_fd < 0 ) {
        log_error("[%s][%s:%d] Failed to start IPC server\n", __FILE__,__func__,__LINE__);\
        shm_scheduler_destroy(g_scheduler);
        return 1;
    }

    // Main server loop to connect, handle one client at a time.
    while(g_running) {

        // Block until connect to a client.
        log_out("[%s][%s:%d] Waiting for client connection...\n", __FILE__,__func__,__LINE__);
        int client_fd = c_MainDaemon->ipc_server_accept(server_fd);
        if(client_fd < 0) {
            log_error("[%s][%s:%d] Failed to accept client connection\n", __FILE__,__func__,__LINE__);
            continue;   // Skip to next iteration and try to accept again.
        }

        char c_buffer[MINISCHED_MAX_CMD_SIZE] = {0};
        
        // Read a single line from the client.
        log_out("[%s][%s:%d] Receiving command from client...\n", __FILE__,__func__,__LINE__);
        int i_bytes = c_MainDaemon->ipc_recv_line(client_fd, c_buffer, sizeof(c_buffer));

        char resp[MINISCHED_MAX_STRING_SIZE] = {0};

        if(i_bytes > 0) {
            if( strncmp(c_buffer, "ADD", 3) == 0 ) {
            log_out("[%s][%s:%d] Handling ADD command...\n", __FILE__,__func__,__LINE__);
            // Handle the "ADD" command.
                char *command = c_buffer + 3;
                while( *command == ' ') {
                    command++;
                }

                // Get the command after "ADD".
                size_t cmd_len = strlen(command);
                while( cmd_len > 0 && (command[cmd_len-1] == '\n' || command[cmd_len-1] == '\r') ) {
                    command[cmd_len-1] = '\0';
                    cmd_len--;
                }

                if( cmd_len == 0 ) {
                    strncpy(resp, "ERR EMPTY COMMAND\n", sizeof(resp) - 1);
                } else {
                // Enqueue the job to the job queue.
                    pthread_mutex_lock(&g_scheduler->queue.mutex);

                    if( job_queue_is_full(&g_scheduler->queue) ) {
                        log_error("[%s][%s:%d] Job queue is full\n", __FILE__,__func__,__LINE__);
                        strncpy(resp, "ERR QUEUE IS FULL\n", sizeof(resp) - 1);
                    } else if( g_scheduler->history_jobs_size >= MINISCHED_MAX_JOBS ) {
                        log_error("[%s][%s:%d] History job table is full\n", __FILE__,__func__,__LINE__);
                        strncpy(resp, "ERR JOB TABLE IS FULL\n", sizeof(resp) - 1);
                    } else {
                        job_t job = {};
                        job.id = g_scheduler->next_job_id++;
                        strncpy(job.command, command, sizeof(job.command) - 1);
                        job.state = JOB_PENDING;
                        get_local_time_string(job.created_time, sizeof(job.created_time), time(NULL), LOCAL_TIME_FORMAT_STRING);

                        if( job_queue_push(&g_scheduler->queue, &job) == 0 ) {
                            log_out("[%s][%s:%d] Enqueued done job ID[%d]: %s\n", __FILE__,__func__,__LINE__, job.id, job.command);
                            g_scheduler->history_jobs[g_scheduler->history_jobs_size++] = job;
                            
                            pthread_cond_signal(&g_scheduler->queue.cond);          // Signal the worker thread that a new job is available.
                            
                            snprintf(resp, sizeof(resp), "ENQUEUE JOB ID[%d] DONE\n", job.id);
                        } else {
                            log_error("[%s][%s:%d] Failed to enqueue job\n", __FILE__,__func__,__LINE__);
                            strncpy(resp, "ENQUEUE JOB FAILED\n", sizeof(resp) - 1);
                        }
                    }

                    pthread_mutex_unlock(&g_scheduler->queue.mutex);
                }
            } else if( strncmp(c_buffer, "STATUS", 6) == 0 ) {
            // Handle the "STATUS" command to return the status of all jobs in the history.
                log_out("[%s][%s:%d] Handling STATUS command...\n", __FILE__,__func__,__LINE__);
                pthread_mutex_lock(&g_scheduler->queue.mutex);

                if( g_scheduler->history_jobs_size == 0 ) {
                    log_error("[%s][%s:%d] No jobs in history\n", __FILE__,__func__,__LINE__);
                    strncpy(resp, "ERR JOB TABLE IS EMPTY\n", sizeof(resp) - 1);
                } else {
                    if( scheduler_stats_update(g_scheduler) != 0 ) {
                        log_error("[%s][%s:%d] Failed to update scheduler stats\n", __FILE__,__func__,__LINE__);
                    }

                    int offset = snprintf(resp, sizeof(resp),
                                "\r\nTOTAL = %d | DONE = %d | FAILED = %d | RUNNING = %d | PENDING = %d\r\n",
                                g_scheduler->stats.total_jobs,
                                g_scheduler->stats.completed_jobs,
                                g_scheduler->stats.failed_jobs,
                                g_scheduler->stats.running_jobs,
                                g_scheduler->stats.pending_jobs);

                    // int offset = snprintf(resp, sizeof(resp), "TOTAL = %d\r\n", g_scheduler->history_jobs_size);
                    if( offset > 0 ) {
                        offset += snprintf( resp + offset,
                                        sizeof(resp) - offset,
                                        "ID  PID     STATE           CREATED TIME            STARTED TIME            FINISHED TIME           COMMAND\r\n" );
                    }

                    for( size_t job_index = 0; (job_index < g_scheduler->history_jobs_size) && (offset > 0) && (offset < int(sizeof(resp) - 1)); job_index++) {
                        job_t job = g_scheduler->history_jobs[job_index];
                        int n = snprintf( resp + offset,
                                        sizeof(resp) - offset,
                                        "%-4d%-8d%-16s%-24s%-24s%-24s%s\r\n",
                                        job.id,
                                        job.pid,
                                        job_state_to_string(job.state),
                                        job.created_time,
                                        job.started_time,
                                        job.finished_time,
                                        job.command );
                        
                        // If there is an error or no more space to write, break the loop.
                        if(n <= 0) {
                            break;
                        }

                        offset += n;
                    }

                    if(offset < int(sizeof(resp) - 1)) {
                        resp[offset] = '\n';
                    } else {
                        resp[sizeof(resp) - 2] = '\n';
                    }

                    resp[sizeof(resp) - 1] = '\0';
                }

                pthread_mutex_unlock(&g_scheduler->queue.mutex);
            } else if( strncmp(c_buffer, "STOP", 4) == 0 ) {
            // Handle the "STOP" command to gracefully shutdown the daemon.
                log_out("[%s][%s:%d] Handling STOP command...\n", __FILE__,__func__,__LINE__);
                g_running = 0;
                strncpy(resp, "STOPPING DAEMON\n", sizeof(resp) - 1);
            } else {
            // If the command is invalid, send an "UNKNOWN_CMD" response.
                log_error("[%s][%s:%d] UNKNOWN COMMAND: %s\n", __FILE__,__func__,__LINE__, c_buffer);
                strncpy(resp, "UNKNOWN_CMD\n", sizeof(resp) - 1);
            }

            resp[sizeof(resp) - 1] = '\0';

            // Send the response back to the client.
            log_out("[%s][%s:%d] Sending response to client.\n", __FILE__,__func__,__LINE__);
            c_MainDaemon->ipc_send_all(client_fd, resp, strlen(resp));
        }

        // Close the connection to prepare for the next one.
        close(client_fd);
    }

// Gracefully shutdown the worker thread and clean up resources.
    log_out("[%s][%s:%d] Gracefully shutting down ...\n", __FILE__,__func__,__LINE__);
    pthread_mutex_lock(&g_scheduler->queue.mutex);

    g_scheduler->shutting_down = 1;

    pthread_cond_broadcast(&g_scheduler->queue.cond);    // Wake up all waiting threads to allow them to exit.
    pthread_mutex_unlock(&g_scheduler->queue.mutex);

    // Wait for all worker processes to exit.
    for( int i = 0; i < g_scheduler->num_workers; i++ ) {
        if( g_scheduler->worker_pids[i] > 0 ) {
            int status;
            waitpid(g_scheduler->worker_pids[i], &status, 0);
            log_out("[%s][%s:%d] Worker [PID: %d] exited with status %d\n",
                    __FILE__,__func__,__LINE__, g_scheduler->worker_pids[i], WEXITSTATUS(status));
        }
    }

    // Clean up resources.
    MainDaemon::remove_pid_file(MINISCHED_PID_FILEPATH);
    shm_scheduler_destroy(g_scheduler);
    unlink(config.socket_path);
    close(server_fd);
    
    log_out("[%s][%s:%d] MiniScheduler daemon exited.\n", __FILE__,__func__,__LINE__);
    return 0;
}