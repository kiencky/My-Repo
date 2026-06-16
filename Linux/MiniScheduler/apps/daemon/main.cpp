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

#include "../../include/ipc/unix_socket.h"
#include "../../include/ipc/protocol.h"
#include "../../include/config/config.h"
#include "../../include/config/config_loader.h"

static volatile sig_atomic_t g_running = 1;     // Flag to control the main loop.

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
        log_error("[%s:%d] Daemonize failed",__func__,__LINE__);
        return 1;
    }

    // Set up signal handlers for graceful shutdown.
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

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
        int i_bytes = c_MainDaemon->ipc_recv_line(client_fd, c_buffer, sizeof(c_buffer));

        char resp[MINISCHED_MAX_CMD_SIZE] = {0};
        if(i_bytes > 0) {
            if( strncmp(c_buffer, "ADD", 3) == 0 ) {
                strncpy(resp, "ADD Processing...\n", sizeof(resp) - 1);
            } else if( strncmp(c_buffer, "STATUS", 6) == 0 ) {
                strncpy(resp, "STATUS Processing...\n", sizeof(resp) - 1);
            } else {
                // Send UNKNOWN_CMD message to client.
                strncpy(resp, "UNKNOWN_CMD\n", sizeof(resp) - 1);
            }
            c_MainDaemon->ipc_send_all(client_fd, resp, strlen(resp));
        }

        // Close the connection to prepare for the next one.
        close(client_fd);
    }

    close(server_fd);
    unlink(config.socket_path);
    MainDaemon::remove_pid_file(MINISCHED_PID_FILEPATH);
    
    log_out("MiniScheduler daemon stopped.\n");
    return 0;
}