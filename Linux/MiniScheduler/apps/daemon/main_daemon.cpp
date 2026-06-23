//==================================================================================================================
// main.cpp
// Note: Main daemon process for the MiniScheduler project.
//       Running in background, initialize the IPC(Unix domain socket).
// Flow: Create socket -> Listen client -> Connect(accept) client -> Read from client -> Close socket
//==================================================================================================================

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "../../include/ipc/unix_socket.h"
#include "../../include/ipc/protocol.h"
#include "../../include/config/config.h"
#include "../../include/config/config_loader.h"
#include "../../include/job/job.h"


extern char g_log_path[1024];    // Global variable to hold the log path, defined in unix_socket.cpp

// volatile is used to prevent compiler optimizations so that main process can access the variable stored in memory instead of register cache.
// sig_atomic_t is used to ensure that the variable is accessed atomically, which is important for signal handlers to avoid race conditions.
static volatile sig_atomic_t g_running = 1;     // Flag to control the main loop.

// Struct to save the scheduler information.
typedef struct {
    job_queue_t queue;
    pthread_t worker_thread;
    size_t next_job_id;
    job_t history_jobs[MINISCHED_MAX_JOBS];
    size_t history_jobs_size;
    int shutting_down;          // Flag to indicate if the scheduler is shutting down.
} scheduler_t;

static scheduler_t g_scheduler;     // Global scheduler instance.

/// @brief  Find a job in the history jobs array by its ID.
/// @param  job_id 
/// @return 
/// @note
static job_t* find_job_in_history_jobs(size_t job_id)
{
    for( size_t i = 0; i < g_scheduler.history_jobs_size; i++ ) {
        if(g_scheduler.history_jobs[i].id == job_id) {
            return &g_scheduler.history_jobs[i];
        }
    }

    return NULL;
}

/// @brief  Worker thread main function to execute jobs from the job queue.
/// @param  
/// @return 
/// @note
static void* worker_main(void* arg)
{
    log_out("[%s][%s:%d] Starting worker thread...\n", __FILE__,__func__,__LINE__);

    // Silence the unused parameter warning.
    (void)arg;

    // Worker thread main loop to execute jobs from the job queue.
    while(1) {
        pthread_mutex_lock(&g_scheduler.queue.mutex);

        // Wait until there is a job in the queue or the scheduler is shutting down.
        while( job_queue_is_empty(&g_scheduler.queue) && !g_scheduler.shutting_down ) {
            log_out("[%s][%s:%d] Worker thread is waiting for jobs...\n", __FILE__,__func__,__LINE__);
            pthread_cond_wait(&g_scheduler.queue.cond, &g_scheduler.queue.mutex);
        }

        // If the queue is empty and the scheduler is shutting down, break the loop to allow the thread to exit.
        if( job_queue_is_empty(&g_scheduler.queue) && g_scheduler.shutting_down ) {
            log_out("[%s][%s:%d] Worker thread exits.\n", __FILE__,__func__,__LINE__);
            pthread_mutex_unlock(&g_scheduler.queue.mutex);
            break;
        }

        job_t current_job;

        // Pop a job from the queue. If it fails, unlock the mutex and continue to the next iteration.
        if( job_queue_pop(&g_scheduler.queue, &current_job) == -1 ) {
            log_error("[%s][%s:%d] Failed to pop job from queue\n", __FILE__,__func__,__LINE__);
            pthread_mutex_unlock(&g_scheduler.queue.mutex);
            continue;
        }

        current_job.state = JOB_RUNNING;
        get_local_time_string(current_job.started_time, sizeof(current_job.started_time), time(NULL), LOCAL_TIME_FORMAT_STRING);
        current_job.pid = getpid();

        job_t *history_job = find_job_in_history_jobs(current_job.id);
        if( history_job != NULL ) {
            history_job->state = current_job.state;
            strncpy(history_job->started_time, current_job.started_time, sizeof(history_job->started_time) - 1);
            history_job->pid = current_job.pid;
        }

        pthread_mutex_unlock(&g_scheduler.queue.mutex);

        // Execute the job command using system().
        // Redirect the output to a command log file.
        char sys_cmd[MINISCHED_MAX_CMD_SIZE+MINISCHED_MAX_STRING_SIZE] = {0};
        char cmd_log_path[1024] = {0};
        int log_flag = 0;
        if(g_log_path[0] != '\0') {
            // Insert "_cmd" after the filename.
            const char *last_dot = strrchr(g_log_path, '.');
            if( last_dot != NULL ) {
                log_flag = 1;
                int dir_len = last_dot - g_log_path;  // not include the '.'.
                snprintf(cmd_log_path, sizeof(cmd_log_path), "%.*s_cmd%s", dir_len, g_log_path, last_dot);
            }

            int fd = open(cmd_log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if( fd >= 0 ) {
                log_flag = 1;
                dprintf(fd, "\nExecuting job ID[%d]: %s\n", current_job.id, current_job.command);
                snprintf(sys_cmd, sizeof(sys_cmd), "%s >> %s 2>&1", current_job.command, cmd_log_path);
                close(fd);
            }
        }
        
        if(!log_flag) {
            snprintf(sys_cmd, sizeof(sys_cmd), "%s", current_job.command);
        }

        int return_code = system(sys_cmd);

        pthread_mutex_lock(&g_scheduler.queue.mutex);

        current_job.exit_code = return_code;
        current_job.state = (return_code == 0) ? JOB_COMPLETED : JOB_FAILED;
        get_local_time_string(current_job.finished_time, sizeof(current_job.finished_time), time(NULL), LOCAL_TIME_FORMAT_STRING);

        if( history_job != NULL ) {
            history_job->state = current_job.state;
            history_job->exit_code = current_job.exit_code;
            strncpy(history_job->finished_time, current_job.finished_time, sizeof(history_job->finished_time) - 1);
        }

        pthread_mutex_unlock(&g_scheduler.queue.mutex);
    }

    log_out("[%s][%s:%d] Exiting worker thread...\n", __FILE__,__func__,__LINE__);
    return NULL;
}

//-------------------------------------------
// Note: Handle termination signals to allow graceful shutdown of the daemon.
//
static void handle_signal(int signo) {
    if( signo == SIGINT || signo == SIGTERM ) {
        log_out("[%s][%s:%d] Temination signal received, shutting down the daemon...\n", __FILE__,__func__,__LINE__);
        g_running = 0;
    }
}

// Daemon main function.
int main()
{
    printf("[%s][%s:%d] MiniScheduler daemon starting...\n", __FILE__,__func__,__LINE__);
    MainDaemon *c_MainDaemon = MainDaemon::newInstance();

    // Get the absolute log path.
    char log_path[1024] = {0};
    if( log_path_init() != 0 ) {
        printf("[%s][%s:%d] Failed to initialize log path\n",__FILE__,__func__,__LINE__);
        return 1;
    }
    
    // Create or clear the log file.
    int log_fd = open(log_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if( log_fd >= 0 ) close(log_fd);

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

    // Set up signal handlers for graceful shutdown.
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);


    memset(&g_scheduler, 0, sizeof(g_scheduler));
    g_scheduler.next_job_id = 1;

    // Initialize the job queue.
    log_out("[%s][%s:%d] Initializing job queue...\n", __FILE__,__func__,__LINE__);
    if( job_queue_init(&g_scheduler.queue) != 0 ) {
        log_error("[%s][%s:%d] Failed to initialize job queue\n", __FILE__,__func__,__LINE__);
        return 1;
    }

    // Create and start the worker thread.
    log_out("[%s][%s:%d] Creating worker thread...\n", __FILE__,__func__,__LINE__);
    if( pthread_create(&g_scheduler.worker_thread, NULL, worker_main, NULL) != 0 ) {
        log_error("[%s][%s:%d] Failed to create worker thread\n", __FILE__,__func__,__LINE__);
        return 1;
    }

    // Create and start listening on the Unix domain socket.
    log_out("[%s][%s:%d] Starting IPC server on socket: %s\n", __FILE__,__func__,__LINE__, config.socket_path);
    int server_fd = c_MainDaemon->ipc_server_listen(config.socket_path);
    if( server_fd < 0 ) {
        log_error("[%s][%s:%d] Failed to start IPC server\n", __FILE__,__func__,__LINE__);
        return 1;   // If listening failed, exit here.
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
                    pthread_mutex_lock(&g_scheduler.queue.mutex);

                    if( job_queue_is_full(&g_scheduler.queue) ) {
                        log_error("[%s][%s:%d] Job queue is full\n", __FILE__,__func__,__LINE__);
                        strncpy(resp, "ERR QUEUE IS FULL\n", sizeof(resp) - 1);
                    } else if( g_scheduler.history_jobs_size >= MINISCHED_MAX_JOBS ) {
                        log_error("[%s][%s:%d] History job table is full\n", __FILE__,__func__,__LINE__);
                        strncpy(resp, "ERR JOB TABLE IS FULL\n", sizeof(resp) - 1);
                    } else {
                        job_t job = {};
                        job.id = g_scheduler.next_job_id++;
                        strncpy(job.command, command, sizeof(job.command) - 1);
                        job.state = JOB_PENDING;
                        get_local_time_string(job.created_time, sizeof(job.created_time), time(NULL), LOCAL_TIME_FORMAT_STRING);

                        if( job_queue_push(&g_scheduler.queue, &job) == 0 ) {
                            log_out("[%s][%s:%d] Enqueued done job ID[%d]: %s\n", __FILE__,__func__,__LINE__, job.id, job.command);
                            g_scheduler.history_jobs[g_scheduler.history_jobs_size++] = job;
                            
                            pthread_cond_signal(&g_scheduler.queue.cond);          // Signal the worker thread that a new job is available.
                            
                            snprintf(resp, sizeof(resp), "ENQUEUE JOB ID[%d] DONE\n", job.id);
                        } else {
                            log_error("[%s][%s:%d] Failed to enqueue job\n", __FILE__,__func__,__LINE__);
                            strncpy(resp, "ENQUEUE JOB FAILED\n", sizeof(resp) - 1);
                        }
                    }

                    pthread_mutex_unlock(&g_scheduler.queue.mutex);
                }
            } else if( strncmp(c_buffer, "STATUS", 6) == 0 ) {
            // Handle the "STATUS" command to return the status of all jobs in the history.
                log_out("[%s][%s:%d] Handling STATUS command...\n", __FILE__,__func__,__LINE__);
                pthread_mutex_lock(&g_scheduler.queue.mutex);

                if( g_scheduler.history_jobs_size == 0 ) {
                    log_error("[%s][%s:%d] No jobs in history\n", __FILE__,__func__,__LINE__);
                    strncpy(resp, "ERR JOB TABLE IS EMPTY\n", sizeof(resp) - 1);
                } else {
                    int offset = snprintf(resp, sizeof(resp), "TOTAL = %d\r\n", g_scheduler.history_jobs_size);
                    if( offset > 0 ) {
                        offset += snprintf( resp + offset,
                                    sizeof(resp) - offset,
                                    "ID  PID     STATE           CREATED TIME            STARTED TIME            FINISHED TIME           COMMAND\r\n" );
                    }

                    for( size_t job_index = 0; (job_index < g_scheduler.history_jobs_size) && (offset > 0) && (offset < int(sizeof(resp) - 1)); job_index++) {
                        job_t job = g_scheduler.history_jobs[job_index];
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

                pthread_mutex_unlock(&g_scheduler.queue.mutex);
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
    pthread_mutex_lock(&g_scheduler.queue.mutex);

    g_scheduler.shutting_down = 1;

    pthread_cond_broadcast(&g_scheduler.queue.cond);    // Wake up all waiting threads to allow them to exit.
    pthread_mutex_unlock(&g_scheduler.queue.mutex);

    pthread_join(g_scheduler.worker_thread, NULL);      // Wait for the worker thread to finish.
    pthread_cond_destroy(&g_scheduler.queue.cond);
    pthread_mutex_destroy(&g_scheduler.queue.mutex);

    // Clean up the PID file, socket file and close the server socket.
    MainDaemon::remove_pid_file(MINISCHED_PID_FILEPATH);
    unlink(config.socket_path);
    close(server_fd);
    
    log_out("[%s][%s:%d] MiniScheduler daemon exited.\n", __FILE__,__func__,__LINE__);
    return 0;
}