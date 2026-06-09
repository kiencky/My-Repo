//=================================================================
// Protocol.h
//=================================================================

#ifndef PROTOCOL_H
#define PROTOCOL_H


#define MINISCHED_SOCKET_PATH   "/tmp/minisched.sock"    // Path for the Unix domain socket used for daemon?client communication.
#define MINISCHED_MAX_CMD_SIZE  1024                     // Maximum length for a single command line sent between client and daemon.

#define PID_FILEPATH            "/var/run/minisched.pid"

#endif  // PROTOCOL_H