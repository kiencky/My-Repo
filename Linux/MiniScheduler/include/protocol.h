//=================================================================
// Protocol.h
//=================================================================

#ifndef PROTOCOL_H
#define PROTOCOL_H


#define MINISCHED_SOCKET_PATH       "/tmp/minisched.sock"    // Path for the Unix domain socket used for daemon?client communication.
#define MINISCHED_MAX_CMD_SIZE      1024                     // Maximum length for a single command line sent between client and daemon.
#define MINISCHED_MAX_STRING_SIZE   1024

#define MINISCHED_PID_FILEPATH      "/var/run/minisched.pid"
#define MINISCHED_CONFIG_FILEPATH   "/etc/minisched.conf"
#define MINISCHED_LOG_FILEPATH      "/var/log/minisched.log"

#endif  // PROTOCOL_H