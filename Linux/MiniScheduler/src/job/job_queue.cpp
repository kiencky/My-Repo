//==========================================================
// File: job_queue.cpp
// Note: In-memory job queue implementation for Milestone 3.
//==========================================================

#include <string.h>

#include "../../include/job/job.h"
#include "../../include/ipc/protocol.h"

///-------------------------------------------
// Note: Initialize the job queue.
//
int job_queue_init(job_queue_t *queue)
{
    if( queue == NULL ) {
        log_error("[%s:%d] Invalid argument\n",__func__,__LINE__);
        return -1;
    }

    // Initialize default values.
    memset(queue, 0, sizeof(*queue));

    if( pthread_mutex_init(&queue->mutex, NULL) != 0 ) {
        log_error("[%s:%d] Failed to initialize mutex\n",__func__,__LINE__);
        return -1;
    }

    if( pthread_cond_init(&queue->cond, NULL) != 0 ) {
        log_error("[%s:%d] Failed to initialize condition variable\n",__func__,__LINE__);
        pthread_mutex_destroy(&queue->mutex);
        return -1;
    }

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
        log_error("[%s:%d] Invalid argument\n",__func__,__LINE__);
        return -1;
    }

    if( job_queue_is_full(queue) ) {
        log_error("[%s:%d] Job queue is full\n",__func__,__LINE__);
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
        log_error("[%s:%d] Invalid argument\n",__func__,__LINE__);
        return -1;
    }

    if( job_queue_is_empty(queue) ) {
        log_error("[%s:%d] Job queue is empty\n",__func__,__LINE__);
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
