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
#include "../../scheduler/job.h"

static volatile sig_atomic_t g_running = 1;     // Flag to control the main loop.
==================================================

typedef struct {
    job_queue_t queue;
    pthread_t worker_thread;
    int next_job_id;
    int shutting_down;

    // Milestone 3 status history in daemon memory.
    job_t history[MINISCHED_MAX_JOBS];
    int history_size;
} scheduler_ctx_t;

static scheduler_ctx_t g_scheduler;

static job_t* find_job_in_history_locked(int job_id)
{
    for( int i = 0; i < g_scheduler.history_size; i++ ) {
        if( g_scheduler.history[i].id == job_id ) {
            return &g_scheduler.history[i];
        }
    }

    return NULL;
}

static void* worker_main(void *arg)
{
    (void)arg;

    while(1) {
        pthread_mutex_lock(&g_scheduler.queue.mutex);

        while( job_queue_is_empty(&g_scheduler.queue) && !g_scheduler.shutting_down ) {
            pthread_cond_wait(&g_scheduler.queue.cond, &g_scheduler.queue.mutex);
        }

        if( g_scheduler.shutting_down && job_queue_is_empty(&g_scheduler.queue) ) {
            pthread_mutex_unlock(&g_scheduler.queue.mutex);
            break;
        }

        job_t current_job;
        if( job_queue_pop(&g_scheduler.queue, &current_job) != 0 ) {
            pthread_mutex_unlock(&g_scheduler.queue.mutex);
            continue;
        }

        current_job.state = JOB_RUNNING;
        current_job.started_time = time(NULL);
        current_job.pid = getpid();

        job_t *history_job = find_job_in_history_locked(current_job.id);
        if( history_job != NULL ) {
            history_job->state = current_job.state;
            history_job->started_time = current_job.started_time;
            history_job->pid = current_job.pid;
        }

        pthread_mutex_unlock(&g_scheduler.queue.mutex);

        int rc = system(current_job.command);

        pthread_mutex_lock(&g_scheduler.queue.mutex);

        current_job.exit_code = rc;
        current_job.finished_time = time(NULL);
        current_job.state = (rc == 0) ? JOB_COMPLETED : JOB_FAILED;

        history_job = find_job_in_history_locked(current_job.id);
        if( history_job != NULL ) {
            history_job->state = current_job.state;
            history_job->finished_time = current_job.finished_time;
            history_job->exit_code = current_job.exit_code;
        }

        pthread_mutex_unlock(&g_scheduler.queue.mutex);
    }

    return NULL;
}
=====================================================
//-------------------------------------------
// Note: Handle termination signals to allow graceful shutdown of the daemon.
//
static void handle_signal(int signo) {
    if( signo == SIGINT || signo == SIGTERM ) {
        g_running = 0;
    }
}

int main()
{
    // Load config data before daemonize().
    minisched_config_t config;
    config_load(MINISCHED_CONFIG_FILEPATH, &config);

    // Clear the log file at the start.
    int log_fd = open(MINISCHED_LOG_FILEPATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if( log_fd >= 0 ) close(log_fd);

    MainDaemon *c_MainDaemon = MainDaemon::newInstance();

    log_out("Starting the MiniScheduler daemon...\n");
    // Daemonize the process to run in the background.
    if( MainDaemon::daemonize(MINISCHED_PID_FILEPATH) != 0 ) {
        log_error("[%s:%d] Daemonize failed\n",__func__,__LINE__);
        return 1;
    }

    // Set up signal handlers for graceful shutdown.
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

==================================================
    memset(&g_scheduler, 0, sizeof(g_scheduler));
    g_scheduler.next_job_id = 1;

    if( job_queue_init(&g_scheduler.queue) != 0 ) {
        log_error("[%s:%d] init job queue failed\n", __func__, __LINE__);
        return 1;
    }

    if( pthread_create(&g_scheduler.worker_thread, NULL, worker_main, NULL) != 0 ) {
        log_error("[%s:%d] create worker thread failed\n", __func__, __LINE__);
        return 1;
    }

=====================================================
    char c_buffer[MINISCHED_MAX_CMD_SIZE] = {0};

    // Create and start listening on the Unix domain socket.
    int server_fd = c_MainDaemon->ipc_server_listen(config.socket_path);
    if( server_fd < 0 ) {
        return 1;   // If listening failed, exit here.
    }

    // Main server loop to connect, handle one client at a time.
    while(g_running) {
        // Block until connect to a client.
        int client_fd = c_MainDaemon->ipc_server_accept(server_fd);
        if(client_fd < 0) {
            continue;   // Skip to next iteration and try to accept again.
        }

        // Read a single line from the client.
=========================
        memset(c_buffer, 0, sizeof(c_buffer));
=============================
        int i_bytes = c_MainDaemon->ipc_recv_line(client_fd, c_buffer, sizeof(c_buffer));

        char resp[MINISCHED_MAX_CMD_SIZE] = {0};
        if(i_bytes > 0) {
            if( strncmp(c_buffer, "ADD", 3) == 0 ) {
===========================================================================
                char *command = c_buffer + 3;
                while( *command == ' ' ) {
                    command++;
                }

                size_t cmd_len = strlen(command);
                while( cmd_len > 0 && (command[cmd_len - 1] == '\n' || command[cmd_len - 1] == '\r') ) {
                    command[cmd_len - 1] = '\0';
                    cmd_len--;
                }

                if( cmd_len == 0 ) {
                    strncpy(resp, "ERR EMPTY_COMMAND\n", sizeof(resp) - 1);
                } else {
                    pthread_mutex_lock(&g_scheduler.queue.mutex);

                    if( job_queue_is_full(&g_scheduler.queue) ) {
                        strncpy(resp, "ERR QUEUE_FULL\n", sizeof(resp) - 1);
                    } else if( g_scheduler.history_size >= MINISCHED_MAX_JOBS ) {
                        strncpy(resp, "ERR JOB_TABLE_FULL\n", sizeof(resp) - 1);
                    } else {
                        job_t job;
                        memset(&job, 0, sizeof(job));
                        job.id = g_scheduler.next_job_id++;
                        strncpy(job.command, command, sizeof(job.command) - 1);
                        job.command[sizeof(job.command) - 1] = '\0';
                        job.state = JOB_PENDING;
                        job.created_time = time(NULL);

                        if( job_queue_push(&g_scheduler.queue, &job) == 0 ) {
                            g_scheduler.history[g_scheduler.history_size++] = job;
                            pthread_cond_signal(&g_scheduler.queue.cond);
                            snprintf(resp, sizeof(resp), "OK job_id=%d\n", job.id);
                        } else {
                            strncpy(resp, "ERR ENQUEUE_FAILED\n", sizeof(resp) - 1);
                        }
                    }

                    pthread_mutex_unlock(&g_scheduler.queue.mutex);
                }
==================================================
            } else if( strncmp(c_buffer, "STATUS", 6) == 0 ) {
============================================================
                pthread_mutex_lock(&g_scheduler.queue.mutex);

                if( g_scheduler.history_size == 0 ) {
                    strncpy(resp, "NO_JOBS\n", sizeof(resp) - 1);
                } else {
                    int offset = snprintf(resp, sizeof(resp), "COUNT=%d ", g_scheduler.history_size);
                    for( int i = 0; i < g_scheduler.history_size && offset > 0 && offset < (int)sizeof(resp) - 1; i++ ) {
                        int n = snprintf(
                            resp + offset,
                            sizeof(resp) - offset,
                            "%d:%s%s",
                            g_scheduler.history[i].id,
                            job_state_to_string(g_scheduler.history[i].state),
                            (i == g_scheduler.history_size - 1) ? "" : ","
                        );

                        if( n <= 0 ) {
                            break;
                        }

                        offset += n;
                    }

                    if( offset < (int)sizeof(resp) - 1 ) {
                        resp[offset++] = '\n';
                        resp[offset] = '\0';
                    } else {
                        resp[sizeof(resp) - 2] = '\n';
                        resp[sizeof(resp) - 1] = '\0';
                    }
                }

                pthread_mutex_unlock(&g_scheduler.queue.mutex);
========================================
            } else {
                // Send UNKNOWN_CMD message to client.
                strncpy(resp, "UNKNOWN_CMD\n", sizeof(resp) - 1);
            }
            c_MainDaemon->ipc_send_all(client_fd, resp, strlen(resp));
        }

        // Close the connection to prepare for the next one.
        close(client_fd);
    }
========================================

    pthread_mutex_lock(&g_scheduler.queue.mutex);
    g_scheduler.shutting_down = 1;
    pthread_cond_broadcast(&g_scheduler.queue.cond);
    pthread_mutex_unlock(&g_scheduler.queue.mutex);

    pthread_join(g_scheduler.worker_thread, NULL);
    pthread_cond_destroy(&g_scheduler.queue.cond);
    pthread_mutex_destroy(&g_scheduler.queue.mutex);

=============================================
    close(server_fd);
    unlink(config.socket_path);
    MainDaemon::remove_pid_file(MINISCHED_PID_FILEPATH);
    
    log_out("MiniScheduler daemon stopped.\n");
    return 0;
}