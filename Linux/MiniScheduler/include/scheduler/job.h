//==========================================================
// File: job.h
// Note: Job struct definition for the MiniScheduler daemon.
//==========================================================

#ifndef JOB_H
#define JOB_H

#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#include "../../include/ipc/protocol.h"

// Job state.
typedef enum {
    JOB_PENDING,
    JOB_RUNNING,
    JOB_COMPLETED,
    JOB_FAILED
} job_state_t;

// Job information.
typedef struct {
    int id;                                 // Job ID.
    char command[MINISCHED_MAX_CMD_SIZE];   // Command to execute.
    job_state_t state;                      // Current state.
    pid_t pid;                              // Worker process ID.
    int exit_code;                          // Exit code after completion.
    time_t created_time;                    // Created time of the job.
    time_t started_time;                    // Started time of the job.
    time_t finished_time;                   // Finished time of the job.
} job_t;

// Job queue.
typedef struct {
    job_t jobs[MINISCHED_MAX_JOBS];     // Array to hold jobs.
    int head;                           // Index of the head (next pop).
    int tail;                           // Index of the tail (next push).
    int size;                           // Current number of jobs.
    pthread_mutex_t mutex;              // Mutex to protect access to the queue.
    pthread_cond_t cond;                // Condition variable to inform job availability.
} job_queue_t;

// class JobManager {
// public:
//     // Note: Creates a new instance of JobManager.
//     static JobManager* newInstance();
//     int init_job_queue(job_queue_t *queue);
// };


/// @brief  Initializes the job queue.
/// @param  queue
/// @return 0  : Success
///         -1 : Error
/// @note   .
int job_queue_init(job_queue_t *queue);

/// @brief  Checks if the job queue is full.
/// @param  queue
/// @return 1  : Full
///         0  : Not full
/// @note   .
int job_queue_is_full(const job_queue_t *queue);

/// @brief  Checks if the job queue is empty.
/// @param  queue
/// @return 1  : Empty
///         0  : Not empty
/// @note   .
int job_queue_is_empty(const job_queue_t *queue);

/// @brief  Pushes a job into the job queue.
/// @param  queue
/// @param  job
/// @return 0  : Success
///         -1 : Error
/// @note   .
int job_queue_push(job_queue_t *queue, const job_t *job);

/// @brief  Pops a job from the job queue.
/// @param  queue
/// @param  job
/// @return 0  : Success
///         -1 : Error
/// @note   .
int job_queue_pop(job_queue_t *queue, job_t *job);

/// @brief  Converts a job state to a string representation.
/// @param  state
/// @return A string representing the job state.
/// @note   .
const char* job_state_to_string(job_state_t state);

#endif  // JOB_H