//==========================================================
// File: job.h
// Note: Job struct definition for the MiniScheduler daemon.
//==========================================================

#ifndef JOB_H
#define JOB_H

#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#include "../../include/ipc/def.h"

// Job state.
typedef enum {
    JOB_PENDING,
    JOB_RUNNING,
    JOB_COMPLETED,
    JOB_FAILED
} job_state_t;

// Job information.
typedef struct {
    size_t id;                              // Job ID.
    char command[MINISCHED_MAX_CMD_SIZE];   // Command to execute.
    job_state_t state;                      // Current state.
    pid_t pid;                              // Worker process ID.
    int exit_code;                          // Exit code after completion.
    char created_time[64];                  // Created time of the job.
    char started_time[64];                  // Started time of the job.
    char finished_time[64];                 // Finished time of the job.
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

// Scheduler statistics for monitoring.
typedef struct {
    int total_jobs;          // Total number of jobs in the queue.
    int pending_jobs;        // Number of pending jobs.
    int running_jobs;        // Number of running jobs.
    int completed_jobs;      // Number of completed jobs.
    int failed_jobs;         // Number of failed jobs.
} scheduler_stats_t;

// History table for tracking all jobs (lives in shared memory alongside queue).
typedef struct {
    job_queue_t queue;                          // The job queue.
    job_t history_jobs[MINISCHED_MAX_JOBS];     // History of all jobs.
    size_t history_jobs_size;                   // Number of jobs in history.
    size_t next_job_id;                         // Next job ID to assign.
    pid_t worker_pids[MINISCHED_MAX_WORKERS];   // PIDs of worker processes.
    int shutting_down;                          // Flag to indicate shutdown.
    int num_workers;                            // Number of active workers.
    scheduler_stats_t stats;                    // Scheduler statistics.
} shared_scheduler_t;

/// @brief  Initializes the job queue.
/// @param  queue
/// @return 0  : Success
///         -1 : Error
/// @note   Including initializing the mutex and condition variable.
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

/// @brief  Updates the scheduler statistics.
/// @param  scheduler
/// @return 0  : Success
///         -1 : Error
/// @note   .
int scheduler_stats_update(shared_scheduler_t *scheduler);

/// @brief  Creates and initializes shared memory for the scheduler.
/// @return Pointer to the shared scheduler,
///         NULL : Failure.
/// @note   Initializes mutex/cond with PTHREAD_PROCESS_SHARED attribute.
shared_scheduler_t* shm_scheduler_create();

/// @brief  Attaches to existing shared memory for the scheduler.
/// @return Pointer to the shared scheduler
///         NULL : Failure.
/// @note   Used by worker processes after fork.
shared_scheduler_t* shm_scheduler_attach();

/// @brief  Detaches from the shared memory (unmaps).
/// @param  scheduler
/// @return 0   : Success
///         -1  : Error.
int shm_scheduler_detach(shared_scheduler_t *scheduler);

/// @brief  Destroys the shared memory (unlinks).
/// @param  scheduler
/// @return 0   : Success
///         -1  : Error.
/// @note   Should only be called by the daemon on shutdown.
int shm_scheduler_destroy(shared_scheduler_t *scheduler);

#endif  // JOB_H