//==============================================================================================
// UnixSocket.cpp
// Note: Unix domain socket.
//==============================================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>

#include "../include/protocol.h"
#include "../include/UnixSocket.h"

//-------------------------------------------
/// @brief Write the msg to stderr stream and print to the terminal as default.
/// @param  
/// @return 
//-------------------------------------------
void log_error(const char *fmt,...)
{
    va_list args;                       // Declare a container to hold the extra arguments passed after fmt.
    va_start(args, fmt);                // Initialize args to store all arguments after fmt.
    fprintf(stderr, "[Error] ");
    vfprintf(stderr, fmt, args);        // args will be substituted into fmt and printed to stderr.
    fprintf(stderr, ": %s\n", strerror(errno));
    va_end(args);                       // Must always call va_end after using va_list to clean up.
}

//-------------------------------------------
/// @brief  Write the msg to stdout stream and print to the terminal as default.
/// @param  
/// @return 
//-------------------------------------------
void log_out(const char *fmt,...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[Log] ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

//-------------------------------------------
/// @brief  Print the usage instructions.
/// @param  
/// @return 
/// @note   stderr: like stdout - but not print in the terminal.
///         fflush: print the messages to the terminal immediately.
//-------------------------------------------
void usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s add \"cmd...\"\n", prog);
    fprintf(stderr, "%s status\n", prog);
    fflush(stderr);
}

//-------------------------------------------
// Note: Creates a new instance of MainDaemon.
//
MainDaemon* MainDaemon::newInstance()
{
    static MainDaemon instance;
    return &instance;
}

//-------------------------------------------
// Note: Ensures that the entire buffer is sent to the specified file descriptor, even if multiple send calls are required due to partial sends.
//
int UnixSocket::ipc_send_all(int fd, const char *buffer, size_t length)
{
    size_t total_sent = 0;

    // Loop until the entire buffer is sent.
    while( total_sent < length ) {
        int sent = write(fd, buffer + total_sent, length - total_sent);
        if( sent < 0 ) {
            log_error("[%s:%d] write error",__func__,__LINE__);
            return -1;
        }

        total_sent += sent;
    }

    return 0;
}

//----------------------------------------------------------
// Note:   Read a byte at a time from the specified file descriptor.
//         The function ensures that the buffer is null-terminated and does not exceed the specified maximum length.
//
int UnixSocket::ipc_recv_line(int fd, char *buffer, size_t max_length)
{
    size_t total_recv = 0;
    
    // Read one byte at a time until newline or buffer limit.
    while( total_recv < max_length - 1 ) {
        char data;
        int bytes_read = read(fd, &data, 1);
        
        if( bytes_read == -1 ) {
            log_error("[%s:%d] read error",__func__,__LINE__);
            return -1;
        } else if( bytes_read == 0 ) {
            // EOF reached, break the loop.
            break;
        }

        buffer[total_recv++] = data;
        
        if( data == '\n' ) {
            // Newline indicates end of line, break the loop.
            break;
        }
    }
    
    buffer[total_recv] = '\0';

    return total_recv;
}

//----------------------------------------------------------
// Note:   Creates a Unix domain socket server that listens for incoming client connections on the specified socket path.
//
int MainDaemon::ipc_server_listen(const char *socket_path)
{
    // Create a Unix domain socket.
    // AF_UNIX specifies Unix domain sockets, SOCK_STREAM indicates a stream-oriented socket (like TCP), 0 to use the default protocol.
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        log_error("[%s:%d] create socket error",__func__,__LINE__);
        return -1;
    }

    struct sockaddr_un addr;        // Structure for Unix domain socket.
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);     // Socket path.

    unlink(socket_path);            // Remove the existing socket file to avoid bind errors.

    // Bind the socket to the specified path.
    if( bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1 ) {
        log_error("[%s:%d] bind error",__func__,__LINE__);
        close(fd);
        return -1;
    }

    // Listen for incoming connections. The maximum number of pending connections is 5.
    if( listen(fd, 5) == -1 ) {
        log_error("[%s:%d] listen error",__func__,__LINE__);
        close(fd);
        return -1;
    }

    return fd;
}

//----------------------------------------------------------
// Note:   Waits and accepts a new client connection on the server socket, returning a file descriptor for the accepted connection.
//         This function blocks until a client connects.
//
int MainDaemon::ipc_server_accept(int server_fd)
{
    // Accept a new client connection. The function blocks until a client connects.
    int client_fd = accept(server_fd, NULL, NULL);
    if( client_fd == -1 ) {
        log_error("[%s:%d] accept error",__func__,__LINE__);
        return -1;
    }

    return client_fd;
}

//---------------------------------------------------
// Note: Uses the kill system call with signal 0 to check exist process.
//
int MainDaemon::is_process_existing(pid_t pid)
{
    if( pid < 0 ) {
        return 0;
    }

    if( kill(pid, 0) == 0 ) {
        return 1;
    }

    return 0;
}

//---------------------------------------------------
// Note: Read PID from a specific file.
//
pid_t MainDaemon::read_pid_file(const char *file_path)
{
    FILE *fp = fopen(file_path, "r");

    if(!fp) {
        if( errno == ENOENT ) {
            // No such file or directory.
            return 0;
        }
        log_error("[%s:%d] fopen error",__func__,__LINE__);
        return -1;
    }

    pid_t pid = 0;

    if( fscanf(fp, "%d", &pid) != 1 ) {
        // Empty file or wrong format.
        log_error("[%s:%d] Empty or wrong format file.",__func__,__LINE__);
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return pid;
}

//-------------------------------------------
// Note: Write the current PID to a specific file.
//
int MainDaemon::write_pid_file(const char *file_path)
{
    FILE *fp = fopen(file_path, "w");

    if(!fp) {
        if( errno == ENOENT ) {
            log_error("[%s:%d] fopen error",__func__,__LINE__);
            return -1;
        }
    }

    pid_t pid = getpid();

    if( fprintf(fp, "%d", pid) != 0 ) {
        log_error("[%s:%d] Write the PID failed.",__func__,__LINE__);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

//-------------------------------------------
// Note: Remove the specific file.
//
int MainDaemon::remove_pid_file(const char *file_path)
{
    if( unlink(file_path) == -1 ) {
        log_error("[%s:%d] remove pid file failed",__func__,__LINE__);
        return -1;
    }

    return 0;
}

//-------------------------------------------
// Note: Check if the daemon is existing by checking the PID file and the process existence.
//
int MainDaemon::is_daemon_existing(const char *file_path)
{
    pid_t pid = read_pid_file(file_path);
    if( pid < 0 ) {
        log_error("[%s:%d] read pid file failed",__func__,__LINE__);
        return -1;
    } else if ( pid == 0) {
        // The pid file is empty, no daemon is running.
        return 0;
    }

    // This process is still running.
    if( is_process_existing(pid) == 0 ) {
        log_out("[%s:%d] This process already exists", __func__, __LINE__);
        return -1;
    }

    // This process is not existing but the pid file is not removed yet, then remove it.
    remove_pid_file(file_path);

    return 0;
}

//-------------------------------------------
// Note: Daemonize the process, make daemon run independently in the background.
//
int MainDaemon::daemonize(const char *file_path)
{
    if( is_daemon_existing(file_path) != 0 ) {
        log_error("[%s:%d] Daemon already exists or error occurred",__func__,__LINE__);
        return -1;
    }

    pid_t pid = fork();

    if( pid < 0 ) {
        log_error("[%s:%d] fork error",__func__,__LINE__);
        return -1;
    }

    if( pid > 0) {
        _exit(0);   // Exit immediately in the parent process to allow the child to run independently.
    }

// The child process.
    // Detach child process from TTY(Controllign terminal).
    if( setsid() < 0 ) {
        log_error("[%s:%d] setsid error",__func__,__LINE__);
        return -1;
    }

    // Change the current working directory to root to avoid blocking unmounting of filesystems.
    if( chdir("/") < 0 ) {
        log_error("[%s:%d] chdir error",__func__,__LINE__);
        return -1;
    }

    // Allow the daemon to have full permissions to create files.
    umask(0);

    close(STDIN_FILENO);     // Close standard input.
    close(STDOUT_FILENO);    // Close standard output.
    close(STDERR_FILENO);    // Close standard error.

    int fd = open("/dev/null", O_RDONLY);    // Redirect standard input, output and error to /dev/null.
    // if( fd != -1 ) {
    //     dup2(fd, STDIN_FILENO);
    //     dup2(fd, STDOUT_FILENO);
    //     dup2(fd, STDERR_FILENO);
    //     if( fd > 2 ) {
    //         close(fd);
    //     }
    // }
    int fd1 = open("/dev/null", O_WRONLY);
    int fd2 = open("/dev/null", O_WRONLY);
    (void)fd;   // Suppress unused variable warning.
    (void)fd1;
    (void)fd2;


    // Write the PID to the file.
    if( write_pid_file(file_path) != 0 ) {
        log_error("[%s:%d] write pid file failed",__func__,__LINE__);
        return -1;
    }

    return 0;
}

//--------------------------------------------
// Note: Creates a new instance of MainClient.
//
MainClient* MainClient::newInstance()
{
    static MainClient instance;
    return &instance;
}

//--------------------------------------------
// Note: Connect to a Unix domain socket server.
//
int MainClient::ipc_client_connect(const char *socket_path)
{
    // Create a Unix domain socket.
    // AF_UNIX specifies Unix domain sockets, SOCK_STREAM indicates a stream-oriented socket (like TCP), 0 to use the default protocol.
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if( fd == -1 ) {
        log_error("[%s:%d] create socket error",__func__,__LINE__);
        return -1;
    }

    struct sockaddr_un addr;        // Structure for Unix domain socket.
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);     // Socket path.

    // Connect to the server socket at the specified path.
    if( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1 ) {
        log_error("[%s:%d] connect error",__func__,__LINE__);
        close(fd);
        return -1;
    }

    return fd;
}