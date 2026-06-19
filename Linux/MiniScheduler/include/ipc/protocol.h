//=================================================================
// Protocol.h
//=================================================================

#ifndef PROTOCOL_H
#define PROTOCOL_H


#define MINISCHED_MAX_CMD_SIZE      1024                // Maximum length for a single command line.
#define MINISCHED_MAX_STRING_SIZE   1024
#define MINISCHED_MAX_JOBS          100                 // Maximum number of jobs that can be handled.

#define MINISCHED_PID_FILEPATH          "/tmp/minisched.pid"
#define MINISCHED_CONFIG_FILEPATH       "config/minisched.conf"
#define MINISCHED_LOG_FILEPATH          "%s/logs/minisched.log"


void log_error(const char *fmt,...);
void log_out(const char *fmt,...);
void usage(const char *prog);
int get_absolute_path(char *log_path, size_t max_length);

#endif  // PROTOCOL_H