//==================================================================================================================
// main_daemon.c
// Note: Main daemon process for the MiniScheduler project.
//       Running in background, initialize the IPC(Unix domain socket).
// Flow: Create socket -> Listen client -> Connect(accept) client -> Read from client -> Close socket
//==================================================================================================================

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

#include "../include/UnixSocket.h"
#include "../include/protocol.h"

int main()
{
    MainDaemon *c_MainDaemon = MainDaemon::newInstance();
    char c_buffer[MINISCHED_MAX_CMD_SIZE] = {0};

    // Create and start listening on the Unix domain socket.
    int server_fd = c_MainDaemon->ipc_server_listen(MINISCHED_SOCKET_PATH);
    if( server_fd < 0 ) {
        return 1;   // If listening failed, exit here.
    }

    // Main server loop to connect, handle one client at a time.
    for(;;) {
        // Block until connect to a client.
        int client_fd = c_MainDaemon->ipc_server_accept(server_fd);
        if(client_fd < 0) {
            continue;   // Skip to next iteration and try to accept again.
        }

        // Read a single line from the client.
        int i_bytes = c_MainDaemon->ipc_recv_line(client_fd, c_buffer, sizeof(c_buffer));

        if(i_bytes > 0) {
            if( strncmp(c_buffer, "ADD", 3) == 0 ) {
                printf("ADD Processing...\n");
            } else if( strncmp(c_buffer, "STATUS", 6) == 0 ) {
                printf("STATUS Processing...\n");
            } else {
                // Send UNKNOWN_CMD message to client.
                const char *resp = "UNKNOWN_CMD\n";
                c_MainDaemon->ipc_send_all(client_fd, resp, strlen(resp));
            }
        }

        // Close the connection to prepare for the next one.
        close(client_fd);
    }

    close(server_fd);
    unlink(MINISCHED_SOCKET_PATH);
    
    return 0;
}