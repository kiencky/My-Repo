//==========================================================
// File: job_queue.cpp
// Note: Shared memory job queue implementation.
//==========================================================

#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../include/job/job.h"
#include "../../include/ipc/protocol.h"

///-------------------------------------------
// Note: Initialize the job queue.
//
int job_queue_init(job_queue_t *queue)
{
    if( queue == NULL ) {
        log_error("[%s][%s:%d] Invalid argument\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    // Initialize default values.
    memset(queue, 0, sizeof(*queue));

    if( pthread_mutex_init(&queue->mutex, NULL) != 0 ) {
        log_error("[%s][%s:%d] Failed to initialize mutex\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    if( pthread_cond_init(&queue->cond, NULL) != 0 ) {
        log_error("[%s][%s:%d] Failed to initialize condition variable\n",__FILE__,__func__,__LINE__);
        pthread_mutex_destroy(&queue->mutex);
        return -1;
    }

    return 0;
}

int job_queue_init(job_queue_t *queue)
{
    if( queue == NULL ) {
        log_error("[%s][%s:%d] Invalid argument\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    // Initialize default values.
    memset(queue, 0, sizeof(*queue));

    // Set up process-shared mutex.
    // Attribute is used to specify the behavior of the mutex.
    // Once the mutex is initialized, the attribute object can be destroyed while the mutex remains it internal settings.
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);       // Set the mutex to be shared between processes.
    pthread_mutexattr_setrobust(&mattr, PTHREAD_MUTEX_ROBUST);          // Make mutex recoverable if the owning process crashes or dies while holding the lock.

    if( pthread_mutex_init(&queue->mutex, &mattr) != 0 ) {
        log_error("[%s][%s:%d] Failed to initialize mutex\n",__FILE__,__func__,__LINE__);
        pthread_mutexattr_destroy(&mattr);
        return -1;
    }
    pthread_mutexattr_destroy(&mattr);

    // Set up process-shared condition variable.
    pthread_condattr_t cattr;
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

    if( pthread_cond_init(&queue->cond, &cattr) != 0 ) {
        log_error("[%s][%s:%d] Failed to initialize condition variable\n",__FILE__,__func__,__LINE__);
        pthread_mutex_destroy(&queue->mutex);
        pthread_condattr_destroy(&cattr);
        return -1;
    }
    pthread_condattr_destroy(&cattr);

    return 0;
}

///-------------------------------------------
/// Note: Check if the job queue is full.
//
int job_queue_is_full(const job_queue_t *queue)
{
    return (queue->size >= MINISCHED_MAX_JOBS);
}

int job_queue_is_empty(const job_queue_t *queue)
{
    return (queue->size == 0);
}

int job_queue_push(job_queue_t *queue, const job_t *job)
{
    if( queue == NULL || job == NULL ) {
        log_error("[%s][%s:%d] Invalid argument\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    if( job_queue_is_full(queue) ) {
        log_error("[%s][%s:%d] Job queue is full\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    queue->jobs[queue->tail] = *job;
    queue->tail = (queue->tail + 1) % MINISCHED_MAX_JOBS;
    queue->size++;

    return 0;
}

int job_queue_pop(job_queue_t *queue, job_t *job)
{
    if( queue == NULL || job == NULL ) {
        log_error("[%s][%s:%d] Invalid argument\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    if( job_queue_is_empty(queue) ) {
        log_error("[%s][%s:%d] Job queue is empty\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    *job = queue->jobs[queue->head];
    queue->head = (queue->head + 1) % MINISCHED_MAX_JOBS;
    queue->size--;

    return 0;
}

const char* job_state_to_string(job_state_t state)
{
    switch(state) {
        case JOB_PENDING:
            return "PENDING";
        case JOB_RUNNING:
            return "RUNNING";
        case JOB_COMPLETED:
            return "DONE";
        case JOB_FAILED:
            return "FAILED";
        default:
            return "UNKNOWN";
    }
}
=====================================================

//-------------------------------------------
// Note: Creates and initializes shared memory for the scheduler.
//       Called by the daemon process only.
//
shared_scheduler_t* shm_scheduler_create()
{
    // Remove existing shared memory if any (cleanup from previous crash).
    shm_unlink(MINISCHED_JOBQUEUE_SHM_NAME);

    int fd = shm_open(MINISCHED_JOBQUEUE_SHM_NAME, O_CREAT | O_RDWR, 0666);
    if( fd < 0 ) {
        log_error("[%s][%s:%d] shm_open error\n",__FILE__,__func__,__LINE__);
        return NULL;
    }

    if( ftruncate(fd, sizeof(shared_scheduler_t)) < 0 ) {
        log_error("[%s][%s:%d] ftruncate error\n",__FILE__,__func__,__LINE__);
        close(fd);
        return NULL;
    }

    shared_scheduler_t *scheduler = (shared_scheduler_t*)mmap(NULL, sizeof(shared_scheduler_t),
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, 0);
    close(fd);

    if( scheduler == MAP_FAILED ) {
        log_error("[%s][%s:%d] mmap error\n",__FILE__,__func__,__LINE__);
        return NULL;
    }

    // Initialize the shared scheduler.
    memset(scheduler, 0, sizeof(*scheduler));
    scheduler->next_job_id = 1;

    // Initialize the job queue.
    if( job_queue_init(&scheduler->queue) != 0 ) {
        log_error("[%s][%s:%d] Failed to initialize job queue\n",__FILE__,__func__,__LINE__);
        munmap(scheduler, sizeof(shared_scheduler_t));
        shm_unlink(MINISCHED_JOBQUEUE_SHM_NAME);
        return NULL;
    }

    return scheduler;
}

//-------------------------------------------
// Note: Attaches to existing shared memory.
//       Called by worker processes after fork.
//
shared_scheduler_t* shm_scheduler_attach()
{
    int fd = shm_open(MINISCHED_JOBQUEUE_SHM_NAME, O_RDWR, 0666);
    if( fd < 0 ) {
        log_error("[%s][%s:%d] shm_open error\n",__FILE__,__func__,__LINE__);
        return NULL;
    }

    shared_scheduler_t *scheduler = (shared_scheduler_t*)mmap(NULL, sizeof(shared_scheduler_t),
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, 0);
    close(fd);

    if( scheduler == MAP_FAILED ) {
        log_error("[%s][%s:%d] mmap error\n",__FILE__,__func__,__LINE__);
        return NULL;
    }

    return scheduler;
}

//-------------------------------------------
// Note: Detaches from shared memory (unmaps).
//
int shm_scheduler_detach(shared_scheduler_t *scheduler)
{
    if( scheduler == NULL ) return -1;

    if( munmap(scheduler, sizeof(shared_scheduler_t)) != 0 ) {
        log_error("[%s][%s:%d] munmap error\n",__FILE__,__func__,__LINE__);
        return -1;
    }

    return 0;
}

//-------------------------------------------
// Note: Destroys mutex/cond and removes shared memory.
//       Called by daemon on shutdown only.
//
int shm_scheduler_destroy(shared_scheduler_t *scheduler)
{
    if( scheduler == NULL ) return -1;

    pthread_mutex_destroy(&scheduler->queue.mutex);
    pthread_cond_destroy(&scheduler->queue.cond);

    munmap(scheduler, sizeof(shared_scheduler_t));
    shm_unlink(MINISCHED_JOBQUEUE_SHM_NAME);

    return 0;
}
