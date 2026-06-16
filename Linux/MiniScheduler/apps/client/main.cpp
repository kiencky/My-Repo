//================================================================================================================================
// main.cpp
// Note: Client process.
//       Parsing CLI command and sending request to daemon via Unix domain socket.
// Flow: Receive CLI command (main() parameters) -> Parse -> Connect socket -> Send and Receive data
//================================================================================================================================

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../../include/ipc/unix_socket.h"
#include "../../include/ipc/protocol.h"
#include "../../include/config/config.h"
#include "../../include/config/config_loader.h"

int main(int argc, char* argv[])
{
    // Load config data before daemonize().
    minisched_config_t config;
    config_load(MINISCHED_CONFIG_FILEPATH, &config);

    MainClient *c_MainClient = MainClient::newInstance();

    if( argc < 2 ) {
        // If the command line has only one parameter, show the usage instruction and return error.
        usage(argv[0]);
        return 1;
    }

    char buffer[MINISCHED_MAX_CMD_SIZE] = {0};

    if( strcmp(argv[1], "add") == 0 ) {
        if( argc < 3 ) {
            // If the command line has less than 3 parameters, show the usage instruction and return error.
            usage(argv[0]);
            return 1;
        } else {
            snprintf(buffer, sizeof(buffer), "ADD %s\n", argv[2]);
        }
    } else if( strcmp(argv[1], "status") == 0) {
        if( argc > 2 ) {
            // If the command line has more than 2 parameters, show the usage instruction and return error.
            usage(argv[0]);
            return 1;
        } else {
            snprintf(buffer, sizeof(buffer), "STATUS\n");
        }
    } else {
        // If the command is invalid, show the usage instruction and return error.
        log_out("[%s:%d]UNKNOWN COMMAND: %s\n",__func__,__LINE__,argv[1]);
        usage(argv[0]);
        return 1;
    }

    // Connect to the daemon via Unix domain socket.
    int client_fd = c_MainClient->ipc_client_connect(config.socket_path);
    
    if( client_fd < 0 ) {
        return 1;
    }

    // Send data to the daemon.
    if( c_MainClient->ipc_send_all(client_fd, buffer, strlen(buffer)) < 0 ) {
        log_out("[%s:%d] Send error\n", __func__, __LINE__);
        close(client_fd);
        return 1;
    }

    char resp[MINISCHED_MAX_CMD_SIZE] = {0};

    // Receive the data from the daemon.
    if( c_MainClient->ipc_recv_line(client_fd, resp, sizeof(resp)) < 0) {
        log_out("[%s:%d] Receive error\n", __func__, __LINE__);
        close(client_fd);
        return 1;
    } else {
        log_out("[%s:%d] Receive: %s\n", __func__, __LINE__, resp);
    }

    close(client_fd);
    
    return 0;
}