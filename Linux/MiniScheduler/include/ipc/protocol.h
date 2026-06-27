//=================================================================
// Protocol.h
//=================================================================

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <time.h>

#define MINISCHED_MAX_CMD_SIZE      1024                // Maximum length for a single command line.
#define MINISCHED_MAX_STRING_SIZE   1024
#define MINISCHED_MAX_JOBS          100                 // Maximum number of jobs that can be handled.

#define __FILE__                        __FILE_NAME__
#define MINISCHED_PID_FILEPATH          "/tmp/minisched.pid"
#define MINISCHED_CONFIG_FILEPATH       "config/minisched.conf"
#define MINISCHED_LOG_FILEPATH          "%s/logs/minisched_%s.log"
#define MINISCHED_TEMP_LOG_FILEPATH     "/tmp/minisched.log"
#define MINISCHED_LOGPATH_SHM_NAME      "/minisched_log_path_shm"
#define MINISCHED_JOBQUEUE_SHM_NAME     "/minisched_job_queue_shm"
#define MINISCHED_MAX_WORKERS           8

typedef char LOCAL_TIME_FORMAT;
    static const char LOCAL_TIME_FORMAT_INT         = 1;  // Format local time as an integer (e.g., 20240101123045).
    static const char LOCAL_TIME_FORMAT_STRING      = 2;  // Format local time as a string (e.g., "2024-01-01 12:30:45").

void log_error(const char *fmt,...);
void log_out(const char *fmt,...);
void usage(const char *prog);
int log_path_init();
int get_absolute_log_path(char *log_path, size_t max_length);
int write_log_path_shm(const char* file_path);
int read_log_path_shm(char *log_path, size_t max_length);
int get_local_time_string(char *buffer, size_t buffer_size,time_t t, LOCAL_TIME_FORMAT format);


#endif  // PROTOCOL_H