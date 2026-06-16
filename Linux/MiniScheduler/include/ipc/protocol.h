//=================================================================
// Protocol.h
//=================================================================

#ifndef PROTOCOL_H
#define PROTOCOL_H


#define MINISCHED_MAX_CMD_SIZE      1024                     // Maximum length for a single command line.
#define MINISCHED_MAX_STRING_SIZE   1024

#define MINISCHED_PID_FILEPATH      "/tmp/minisched.pid"
#define MINISCHED_CONFIG_FILEPATH   "config/minisched.conf"
#define MINISCHED_LOG_FILEPATH      "logs/minisched.log"


void log_error(const char *fmt,...);
void log_out(const char *fmt,...);
void usage(const char *prog);

#endif  // PROTOCOL_H