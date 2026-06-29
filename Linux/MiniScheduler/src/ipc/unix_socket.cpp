//==============================================================================================
// unix_socket.cpp
// Note: Unix domain socket.
//      - Shared memory is used to store the log path, so that both the daemon and the client can write logs to the same file.
//==============================================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>
#include <sys/mman.h>

#include "../../include/ipc/protocol.h"
#include "../../include/ipc/unix_socket.h"

// Structure to hold the log path in shared memory.
typedef struct {
    char log_path[1024];
    size_t initialized;
} shared_log_path_t;

char g_log_path[1024] = {0};    // Global variable to hold the log path.

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
/// @brief Write the msg to stderr stream and print to the terminal as default.
/// @param  
/// @return 
//-------------------------------------------
void log_error(const char *fmt,...)
{
    int log_flag = 0;       // Flag to prevent recursive logging.
    int i_errno = errno;    // Save the errno before performing handling.
    char msg[MINISCHED_MAX_STRING_SIZE];

    va_list args;               // Declare a container to hold the extra arguments passed after fmt.
    va_start(args, fmt);        // Initialize args to store all arguments after fmt.
    vsnprintf(msg, sizeof(msg), fmt, args);      // args will be substituted into fmt and saved to msg.
    
    if( i_errno == 0 ) {        // If errno is 0, just print the message without error description.
        fprintf(stderr, "[Error]%s", msg);
    } else {
        fprintf(stderr, "[Error]%s: %s", msg, strerror(i_errno));
    }

    va_end(args);

    if( log_flag == 1 ) return;
    log_flag = 1;

    // Get the absolute log path.
    if( g_log_path[0] == '\0' ) {
        if( get_absolute_log_path(g_log_path, sizeof(g_log_path)) != 0 ) {
            strncpy(g_log_path, MINISCHED_TEMP_LOG_FILEPATH, sizeof(g_log_path) - 1);
            g_log_path[sizeof(g_log_path) - 1] = '\0';
        }
    }

    // Write to log file.
    int fd = open(g_log_path, O_WRONLY | O_CREAT | O_APPEND, 0664);

    if( fd >= 0 ) {
        if( i_errno == 0 ) {
            dprintf(fd, "[Error]%s", msg);
        } else {
            dprintf(fd, "[Error]%s: %s\n", msg, strerror(i_errno));
        }
        close(fd);
    }

    log_flag = 0;
    return;
}

//-------------------------------------------
/// @brief  Write the msg to stdout stream and print to the terminal as default.
/// @param  
/// @return 
//-------------------------------------------
void log_out(const char *fmt,...)
{
    int log_flag = 0;       // Flag to prevent recursive logging.
    char msg[MINISCHED_MAX_STRING_SIZE];

    va_list args;               // Declare a container to hold the extra arguments passed after fmt.
    va_start(args, fmt);        // Initialize args to store all arguments after fmt.
    vsnprintf(msg, sizeof(msg), fmt, args);      // args will be substituted into fmt and saved to msg.
    fprintf(stdout, "[Log]%s", msg);
    va_end(args);

    if( log_flag == 1 ) return;
    log_flag = 1;

    // Get the absolute log path.
    if( g_log_path[0] == '\0' ) {
        if( get_absolute_log_path(g_log_path, sizeof(g_log_path)) != 0 ) {
            strncpy(g_log_path, MINISCHED_TEMP_LOG_FILEPATH, sizeof(g_log_path) - 1);
            g_log_path[sizeof(g_log_path) - 1] = '\0';
        }
    }

    // Write to log file.
    int fd = open(g_log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if( fd >= 0 ) {
        dprintf(fd, "[Log]%s", msg);
        close(fd);
    }

    log_flag = 0;
    return;
}

/// @brief  Initializes the log path by remove the old shared memory object and create a new one.
/// @param log_path 
/// @param max_length 
/// @return 
int log_path_init()
{
    // If the shared memory object already exists, remove it.
    shm_unlink(MINISCHED_LOGPATH_SHM_NAME);

    memset(g_log_path, 0, sizeof(g_log_path));

    // Get the absolute log path and write it to the shared memory.
    if( get_absolute_log_path(g_log_path, sizeof(g_log_path) - 1) != 0 ) {
        printf("[%s][%s:%d] Failed to get absolute log path\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    log_out("[%s][%s:%d] Log path initialized: %s\n", __FILE__,__func__,__LINE__, g_log_path);
    return 0;
}

/// @brief Get the absolute log path of the current working directory.
/// @param  
/// @return -1 : Error.
///         0  : Success.
int get_absolute_log_path(char *log_path, size_t max_length)
{
    if( log_path == NULL || max_length == 0 ) {
        log_error("[%s][%s:%d] Invalid parameters\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    char c_log_path[1024];
    memset(c_log_path, 0, sizeof(c_log_path));

    // Read the log path from the shared memory.
    // If the log path is not initialized, create a new log path and write it to the shared memery.
    if( read_log_path_shm(c_log_path, max_length) != 0 ) {
        char cwd[512] = {0};

        if( getcwd(cwd, sizeof(cwd)) == NULL || strcmp(cwd, "/") == 0) {
        // If the current working directory is not available or is root, use the temporary log path.
            log_error("[%s][%s:%d] getcwd error or current working directory is root\n",__FILE__,__func__,__LINE__);
            strncpy(c_log_path, MINISCHED_TEMP_LOG_FILEPATH, sizeof(c_log_path) - 1);
            c_log_path[sizeof(c_log_path) - 1] = '\0';
        } else {
        // Create a new log path based on the current working directory.
            char current_time[32] = {0};
            get_local_time_string(current_time, sizeof(current_time), time(NULL), LOCAL_TIME_FORMAT_INT);
            snprintf(c_log_path, sizeof(c_log_path), MINISCHED_LOG_FILEPATH, cwd, current_time);

            // Write the new log path to the shared memory.
            if( write_log_path_shm(c_log_path) == -1 ) {
                log_error("[%s][%s:%d] write log path to shared memory failed\n",__FILE__,__func__,__LINE__);
                return -1;
            }
        }
    }

    if( max_length < strlen(c_log_path) + 1 ) {
        log_error("[%s][%s:%d] log path buffer is too small\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    strncpy(log_path, c_log_path, max_length - 1);
    log_path[max_length-1] = '\0';
    
    return 0;
}

/// @brief  Write the log path to the shared memory.
/// @param  file_path 
/// @return 
/// @note   
int write_log_path_shm(const char* file_path)
{
    printf("[%s][%s:%d] Writing log path to shared memory: %s\n",__FILE__,__func__,__LINE__, file_path);

    if( file_path == NULL && file_path[0] == '\0') {
        log_error("[%s][%s:%d] Invalid file path\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    int fd = shm_open(MINISCHED_LOGPATH_SHM_NAME, O_CREAT | O_RDWR, 0666);
    if( fd < 0 ) {
        log_error("[%s][%s:%d] shm_open error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    if( ftruncate(fd, sizeof(shared_log_path_t)) < 0 ) {
        log_error("[%s][%s:%d] ftruncate error\n",__FILE__,__func__,__LINE__);
        close(fd);
        return -1;
    }

    // Map the shared memory object to the process's address space.
    shared_log_path_t *shared = (shared_log_path_t*)mmap(NULL, sizeof(shared_log_path_t),
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 fd, 0);

    if( shared == MAP_FAILED ) {
        log_error("[%s][%s:%d] mmap error\n",__FILE__,__func__,__LINE__);
        close(fd);
        return -1;
    }

    memset(shared, 0, sizeof(*shared));
    strncpy(shared->log_path, file_path, sizeof(shared->log_path) - 1);
    shared->initialized = 1;

    munmap(shared, sizeof(shared_log_path_t));
    close(fd);

    printf("[%s][%s:%d] Log path written to shared memory successfully.\n",__FILE__,__func__,__LINE__);
    return 0;
}

/// @brief  Read the log path from the shared memory.
/// @param  log_path 
/// @param  max_length 
/// @return 
/// @note   
int read_log_path_shm(char *log_path, size_t max_length)
{
    printf("[%s][%s:%d] Reading log path from shared memory...\n",__FILE__,__func__,__LINE__);

    if( log_path == NULL || max_length == 0 ) {
        log_error("[%s][%s:%d] Invalid parameters\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    int fd = shm_open(MINISCHED_LOGPATH_SHM_NAME, O_RDONLY, 0666);
    if( fd < 0 ) {
        // ENOENT is expected when shared memory hasn't been created yet.
        return -1;
    }

    // Map the shared memory object to the process's address space.
    shared_log_path_t *shared = (shared_log_path_t*)mmap(NULL, sizeof(shared_log_path_t),
                                 PROT_READ,
                                 MAP_SHARED,
                                 fd, 0);

    if( shared == MAP_FAILED ) {
        log_error("[%s][%s:%d] mmap error\n",__FILE__,__func__,__LINE__);
        close(fd);
        return -1;
    }

    if( shared->initialized == 0 || max_length < strlen(shared->log_path) + 1 || shared->log_path[0] == '\0') {
        log_error("[%s][%s:%d] Log path is not initialized or buffer is invalid\n",__FILE__,__func__,__LINE__);
        munmap(shared, sizeof(shared_log_path_t));
        close(fd);
        return -1;
    }
    
    strncpy(log_path, shared->log_path, max_length - 1);
    log_path[max_length-1] = '\0';

    munmap(shared, sizeof(shared_log_path_t));
    close(fd);

    printf("[%s][%s:%d] Log path read from shared memory: %s\n",__FILE__,__func__,__LINE__, log_path);
    return 0;
}

/// @brief  Get the local time string based on the specified format.
/// @param  t
/// @param  format
/// @param  buffer
/// @param  buffer_size
/// @return
/// @note   
int get_local_time_string(char *buffer, size_t buffer_size, time_t t, LOCAL_TIME_FORMAT format)
{
    if( buffer == NULL || buffer_size == 0 ) {
        log_error("[%s][%s:%d] Invalid parameters\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    struct tm *local_time = localtime(&t);
    if( local_time == NULL ) {
        log_error("[%s][%s:%d] localtime error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    switch(format) {
        case LOCAL_TIME_FORMAT_INT:
            snprintf(buffer, buffer_size, "%04d%02d%02d%02d%02d%02d",
                     local_time->tm_year + 1900,
                     local_time->tm_mon + 1,
                     local_time->tm_mday,
                     local_time->tm_hour,
                     local_time->tm_min,
                     local_time->tm_sec);
            break;
        case LOCAL_TIME_FORMAT_STRING:
            snprintf(buffer, buffer_size, "%04d-%02d-%02d %02d:%02d:%02d",
                     local_time->tm_year + 1900,
                     local_time->tm_mon + 1,
                     local_time->tm_mday,
                     local_time->tm_hour,
                     local_time->tm_min,
                     local_time->tm_sec);
            break;
        default:
            log_error("[%s][%s:%d] Unknown time format: %d\n",__FILE__,__func__,__LINE__, format);
            snprintf(buffer, buffer_size, "UNKNOWN TIME FORMAT");
            break;
    }

    buffer[buffer_size - 1] = '\0';  // Ensure null-termination.
    return 0;
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
            log_error("[%s][%s:%d] write error\n",__FILE__,__func__,__LINE__);
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
            log_error("[%s][%s:%d] read error\n",__FILE__,__func__,__LINE__);
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
        log_error("[%s][%s:%d] create socket error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    struct sockaddr_un addr;        // Structure for Unix domain socket.
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);     // Socket path.

    unlink(socket_path);            // Remove the existing socket file to avoid bind errors.

    // Bind the socket to the specified path.
    if( bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1 ) {
        log_error("[%s][%s:%d] bind error\n",__FILE__,__func__,__LINE__);
        close(fd);
        return -1;
    }

    // Listen for incoming connections. The maximum number of pending connections is 5.
    if( listen(fd, 5) == -1 ) {
        log_error("[%s][%s:%d] listen error\n",__FILE__,__func__,__LINE__);
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
        log_error("[%s][%s:%d] accept error\n",__FILE__,__func__,__LINE__);
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
        log_error("[%s][%s:%d] fopen error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    pid_t pid = 0;

    if( fscanf(fp, "%d", &pid) != 1 ) {
        // Empty file or wrong format.
        log_error("[%s][%s:%d] Empty or wrong format file.\n",__FILE__,__func__,__LINE__);
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
        log_error("[%s][%s:%d] fopen error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    pid_t pid = getpid();

    if( fprintf(fp, "%d", pid) < 0 ) {
        log_error("[%s][%s:%d] Write the PID failed.\n",__FILE__,__func__,__LINE__);
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
        if( errno == ENOENT ) {
            return 0;
        }
        log_error("[%s][%s:%d] remove pid file failed\n",__FILE__,__func__,__LINE__);
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
        log_error("[%s][%s:%d] read pid file failed\n",__FILE__,__func__,__LINE__);
        return -1;
    } else if ( pid == 0) {
        // The pid file is empty, no daemon is running.
        return 0;
    }

    // This process is still running.
    if( is_process_existing(pid) == 1 ) {
        log_out("[%s][%s:%d] This process already exists\n",__FILE__,__func__,__LINE__);
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
        log_error("[%s][%s:%d] Daemon already exists or error occurred\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    log_out("[%s][%s:%d] Daemonizing the process...\n", __FILE__,__func__,__LINE__);
    pid_t pid = fork();

    if( pid < 0 ) {
        log_error("[%s][%s:%d] fork error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

// Parent process.
    if( pid > 0) {
        _exit(0);   // Exit immediately in the parent process to allow the child to run independently.
    }

// Child process.
    // Detach child process from TTY(Controlling terminal).
    if( setsid() < 0 ) {
        log_error("[%s][%s:%d] setsid error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    // Change the current working directory to root to avoid blocking unmounting of filesystems.
    if( chdir("/") < 0 ) {
        log_error("[%s][%s:%d] chdir error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    // Allow the daemon to have full permissions to create files.
    umask(0);

    // Redirect stdin to /dev/null: reads return EOF, no terminal input.
    int fd_in = open("/dev/null", O_RDONLY);
    // dup2(oldfd, newfd) to make STDIN_FILENO(0) point to fd_in.
    if( fd_in < 0 || dup2(fd_in, STDIN_FILENO) < 0 ) {
        log_error("[%s][%s:%d] error with stdin\n",__FILE__,__func__,__LINE__);
        return -1;
    }
    if( fd_in > STDERR_FILENO ) close(fd_in);

    // Redirect stdout and stderr to /dev/null: accidental writes are discarded silently.
    int fd_out = open("/dev/null", O_WRONLY);
    // dup2(oldfd, newfd) to make STDOUT_FILENO(1) and STDERR_FILENO(2) point to fd_out.
    if( fd_out < 0 || dup2(fd_out, STDOUT_FILENO) < 0 || dup2(fd_out, STDERR_FILENO) < 0 ) {
        log_error("[%s][%s:%d] error with stdout or stderr\n",__FILE__,__func__,__LINE__);
        return -1;
    }
    if( fd_out > STDERR_FILENO ) close(fd_out);

    // Write the PID to the file.
    if( write_pid_file(file_path) != 0 ) {
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
        log_error("[%s][%s:%d] create socket error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    struct sockaddr_un addr;        // Structure for Unix domain socket.
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);     // Socket path.

    // Connect to the server socket at the specified path.
    if( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1 ) {
        log_error("[%s][%s:%d] connect error\n",__FILE__,__func__,__LINE__);
        close(fd);
        return -1;
    }

    return fd;
}