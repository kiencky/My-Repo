//=================================================================
// config.h
// Note: Runtime configuration for the MiniScheduler daemon.
//=================================================================

#ifndef CONFIG_H
#define CONFIG_H

#define MINISCHED_DEFAULT_SOCKET_PATH   "/tmp/minisched.sock"
#define MINISCHED_DEFAULT_NUM_WORKERS   4
#define MINISCHED_MAX_PATH_SIZE         256

typedef struct {
    char socket_path[MINISCHED_MAX_PATH_SIZE];
    int num_workers;
} minisched_config_t;

#endif // CONFIG_H