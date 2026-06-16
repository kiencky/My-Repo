//===============================================================================
// main_daemon.h
// Note: Declaration of the main daemon function as a Unix domain socket server.
//===============================================================================

#ifndef MAIN_DAEMON_H
#define MAIN_DAEMON_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

class UnixSocket {
public:
    /// @brief  Sends the entire buffer to the specified file descriptor.
    /// @param  fd 
    /// @param  buffer 
    /// @param  length 
    /// @return 0  : Success
    ///         -1 : Error
    /// @note   Ensures that the entire buffer is sent to the specified file descriptor, even if multiple send calls are required due to partial sends.
    int ipc_send_all(int fd, const char *buffer, size_t length);

    /// @brief  Read a line of text from the specified file descriptor.
    /// @param  fd
    /// @param  buffer
    /// @param  max_length
    /// @return Success: Number of bytes read (excluding null terminator)
    ///         -1 : Error
    /// @note   Reads a line of text from the specified file descriptor (because client/server send data line by line).
    ///         The function ensures that the buffer is null-terminated and does not exceed the specified maximum length.
    int ipc_recv_line(int fd, char *buffer, size_t max_length);
};

class MainDaemon : public UnixSocket
{
public:

    /// @brief  Creates a new instance of MainDaemon.
    /// @return .
    /// @note   .
    static MainDaemon* newInstance();

    /// @brief  Creates a Unix domain socket server and listens client connections.
    /// @param  socket_path 
    /// @return Success: fd of the server socket
    ///         -1 : Error
    /// @note   Creates a Unix domain socket server that listens for incoming client connections on the specified socket path.
    int ipc_server_listen(const char *socket_path);

    /// @brief  Accepts a new client connection on the server socket.
    /// @param  server_fd
    /// @return Success: fd of the accepted client connection
    ///         -1 : Error
    /// @note   Waits and accepts a new client connection on the server socket, returning a file descriptor for the accepted connection.
    ///         This function blocks until a client connects.
    int ipc_server_accept(int server_fd);

    /// @brief  Checks if a process with the given PID exists.
    /// @param  pid
    /// @return 1 : Exist.
    ///         0 : Not exist.
    /// @note   Uses the kill system call with signal 0 to check exist process.

    static pid_t read_pid_file(const char *file_path);

    /// @brief  Checks if a process with the given PID exists.
    /// @param  pid 
    /// @return 1 : Exist.
    ///         0 : Not exist.
    /// @note   Uses the kill system call with signal 0 to check exist process.
    static int is_process_existing(pid_t pid);

    /// @brief  Writes the current process's PID to a specific file.
    /// @param  file_path
    /// @return 0  : Success.
    ///         -1 : Error.
    /// @note   .
    static int write_pid_file(const char *file_path);

    /// @brief  Removes the PID file.
    /// @param  file_path
    /// @return 0  : Success.
    ///         -1 : Error.
    /// @note   Uses the unlink system call to remove the specified file.
    static int remove_pid_file(const char *file_path);

    /// @brief  Checks if the daemon is already running by checking the PID file and the process existence.
    /// @param  file_path 
    /// @return 0  : No existing daemon, safe to start a new one.
    ///         -1 : Daemon already exists or error occurred.
    /// @note
    static int is_daemon_existing(const char *file_path);

    /// @brief  Daemonizes the process.
    /// @param file_path 
    /// @return 0  : Success.
    ///         -1 : Error.
    /// @note
    static int daemonize(const char *file_path);
};

class MainClient : public UnixSocket
{
public:

    /// @brief  Creates a new instance of MainClient.
    /// @return .
    /// @note   .
    static MainClient* newInstance();

    /// @brief  Connects to a Unix domain socket server.
    /// @param  socket_path
    /// @return Success: fd of the connected socket
    ///         -1 : Error
    /// @note   Creates a client socket and connects it to the server socket at the specified path.
    int ipc_client_connect(const char *socket_path);
};

#endif  // MAIN_DAEMON_H