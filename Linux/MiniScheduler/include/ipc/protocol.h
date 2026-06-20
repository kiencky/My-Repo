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
#define MINISCHED_LOG_FILEPATH          "%s/logs/minisched_%lld.log"
#define MINISCHED_TEMP_LOG_FILEPATH     "/tmp/minisched.log"
#define MINISCHED_LOGPATH_SHM_NAME      "/minisched_log_path_shm"

void log_error(const char *fmt,...);
void log_out(const char *fmt,...);
void usage(const char *prog);
int log_path_init(char *log_path, size_t max_length);
int get_absolute_log_path(char *log_path, size_t max_length);
int write_log_path_shm(const char* file_path);
int read_log_path_shm(char *log_path, size_t max_length);

#endif  // PROTOCOL_H