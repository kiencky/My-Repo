//================================================================================================================================
// main_client.cpp
// Note: Client process.
//       Parsing CLI command and sending request to deamon via Unix domain socket.
// Flow: Receive CLI command (main() parameters) -> Parse -> Connect socket -> Send and Receive data
//================================================================================================================================

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../include/UnixSocket.h"
#include "../include/protocol.h"

int main(int argc, char* argv[])
{
    MainClient *c_MainClient = MainClient::newInstance();

    if( argc < 2 ) {
        // If the command line has only one parameter, show the usage instruction and return error.
        usage(argv[0]);
        return 1;
    }

    char buffer[MINISCHED_MAX_CMD_SIZE] = {0};

    if( strncmp(argv[1], "add", 3) == 0 ) {
        if( argc < 3 ) {
            // If the command line has less than 3 parameter, show the usage instruction and return error.
            usage(argv[0]);
            return 1;
        } else {
            snprintf(buffer, sizeof(buffer), "ADD %s\n", argv[2]);
        }
    } else if( strncmp(argv[1], "status", 6) == 0) {
        if( argc > 2 ) {
            // If the command line has more 2 parameter, show the usage instruction and return error.
            usage(argv[0]);
            return 1;
        } else {
            snprintf(buffer, sizeof(buffer), "STATUS\n");
        }
    } else {
        // If the command is invalid, show the usage instruction and return error.
        log_error("[%s:%d]UNKNOWN COMMAND: %s\n",__func__,__LINE__,argv[1]);
        usage(argv[0]);
        return 1;
    }

    // Connect to the deamon via Unix domain socket.
    int client_fd = c_MainClient->ipc_client_connect(MINISCHED_SOCKET_PATH);
    
    if( client_fd < 0 ) {
        return 1;
    }

    // Send data to the deamon.
    if( c_MainClient->ipc_send_all(client_fd, buffer, strlen(buffer)) < 0 ) {
        fprintf(stderr, "Send error\n");
        close(client_fd);
        return 1;
    }

    char resp[MINISCHED_MAX_CMD_SIZE] = {0};

    // Receive the data from the deamon.
    if( c_MainClient->ipc_recv_line(client_fd, resp, sizeof(resp)) < 0) {
        fprintf(stderr, "Receive error\n");
        close(client_fd);
        return 1;
    } else {
        printf("Receive: %s\n",resp);
    }

    close(client_fd);
    
    return 0;
}