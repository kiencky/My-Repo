#include <cstring>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock.h"
#include "../include/config/config.h"
#include "../include/config/config_loader.h"
#include "../include/job/job.h"

using namespace std;
using ::testing::Exactly;

MockLogger *g_mockLogger = nullptr;

// Mock implementations (C++ linkage to match def.h)
void log_error(const char *fmt, ...)
{
    (void)fmt;
    if (g_mockLogger != nullptr) {
        g_mockLogger->OnError();
    }
}

namespace {

string write_temp_config(const string &content)
{
    const auto path = filesystem::temp_directory_path() / "minischeduler_ut.conf";
    ofstream ofs(path, ios::trunc);
    ofs << content;
    return path.string();
}

job_t make_job(size_t id, const char *cmd)
{
    job_t j{};
    j.id = id;
    j.state = JOB_PENDING;
    strncpy(j.command, cmd, sizeof(j.command) - 1);
    return j;
}

// Fixture for Config Loader Tests
class UT001_ConfigLoader : public ::testing::Test {
protected:
    MockLogger logger;

    void SetUp() override
    {
        g_mockLogger = &logger;
    }

    void TearDown() override
    {
        g_mockLogger = nullptr;
    }
};

//================================
// UT001: Config Loader Tests
//================================

// TC001: Missing Config File
// Expected: Return default values.
TEST_F(UT001_ConfigLoader, TC001_MissingFileUseDefault)
{
    minisched_config_t config{};

    EXPECT_CALL(logger, OnError()).Times(Exactly(1));
    EXPECT_EQ(config_load("/tmp/not_found_minischeduler.conf", &config), 0);
    EXPECT_STREQ(config.socket_path, MINISCHED_DEFAULT_SOCKET_PATH);
    EXPECT_EQ(config.num_workers, MINISCHED_DEFAULT_NUM_WORKERS);
}

// TC002: Valid Config File
// Expected: Correctly parse socket_path and num_workers.
TEST_F(UT001_ConfigLoader, TC002_ParseSocketAndWorkers)
{
    minisched_config_t config{};
    const string path = write_temp_config(
        "socket_path=/tmp/test.sock\n"
        "num_workers=6\n");

    EXPECT_CALL(logger, OnError()).Times(Exactly(0));
    EXPECT_EQ(config_load(path.c_str(), &config), 0);
    EXPECT_STREQ(config.socket_path, "/tmp/test.sock");
    EXPECT_EQ(config.num_workers, 6);
}

// TC003: Invalid Lines Ignored
// Expected: Invalid lines and negative num_workers are ignored; defaults used.
TEST_F(UT001_ConfigLoader, TC003_InvalidLinesIgnored)
{
    minisched_config_t config{};
    const string path = write_temp_config(
        "# comment\n"
        "invalid_line\n"
        "num_workers=-1\n");

    EXPECT_CALL(logger, OnError()).Times(Exactly(0));
    EXPECT_EQ(config_load(path.c_str(), &config), 0);
    EXPECT_STREQ(config.socket_path, MINISCHED_DEFAULT_SOCKET_PATH);
    EXPECT_EQ(config.num_workers, MINISCHED_DEFAULT_NUM_WORKERS);
}

// Fixture for Job Queue Tests
class UT002_JobQueue : public ::testing::Test {
protected:
    MockLogger logger;

    void SetUp() override
    {
        g_mockLogger = &logger;
    }

    void TearDown() override
    {
        g_mockLogger = nullptr;
    }
};

//================================
// UT002: Job Queue Tests
//================================

// TC001: Initialize with NULL pointer
// Expected: Return -1 and log error.
TEST_F(UT002_JobQueue, TC001_InitNull)
{
    EXPECT_CALL(logger, OnError()).Times(Exactly(1));
    EXPECT_EQ(job_queue_init(nullptr), -1);
}

// TC002: Push and Pop FIFO Order
// Expected: Jobs are popped in the order they were pushed (FIFO).
TEST_F(UT002_JobQueue, TC002_PushPopFIFO)
{
    job_queue_t queue{};
    EXPECT_CALL(logger, OnError()).Times(Exactly(0));

    ASSERT_EQ(job_queue_init(&queue), 0);

    const job_t j1 = make_job(1, "echo 1");
    const job_t j2 = make_job(2, "echo 2");
    ASSERT_EQ(job_queue_push(&queue, &j1), 0);
    ASSERT_EQ(job_queue_push(&queue, &j2), 0);

    job_t out{};
    ASSERT_EQ(job_queue_pop(&queue, &out), 0);
    EXPECT_EQ(out.id, 1);

    ASSERT_EQ(job_queue_pop(&queue, &out), 0);
    EXPECT_EQ(out.id, 2);

    pthread_cond_destroy(&queue.cond);
    pthread_mutex_destroy(&queue.mutex);
}

// TC003: Pop from Empty Queue
// Expected: Return -1 and log error.
TEST_F(UT002_JobQueue, TC003_PopEmpty)
{
    job_queue_t queue{};
    ASSERT_EQ(job_queue_init(&queue), 0);

    job_t out{};
    EXPECT_CALL(logger, OnError()).Times(Exactly(1));
    EXPECT_EQ(job_queue_pop(&queue, &out), -1);

    pthread_cond_destroy(&queue.cond);
    pthread_mutex_destroy(&queue.mutex);
}

// TC004: Scheduler Stats Count
// Expected: Correctly count total, pending, running, completed, and failed jobs.
TEST_F(UT002_JobQueue, TC004_StatsCount)
{
    shared_scheduler_t scheduler{};
    scheduler.history_jobs_size = 5;
    scheduler.history_jobs[0].state = JOB_PENDING;
    scheduler.history_jobs[1].state = JOB_RUNNING;
    scheduler.history_jobs[2].state = JOB_COMPLETED;
    scheduler.history_jobs[3].state = JOB_FAILED;
    scheduler.history_jobs[4].state = static_cast<job_state_t>(10);

    EXPECT_CALL(logger, OnError()).Times(Exactly(0));
    ASSERT_EQ(scheduler_stats_update(&scheduler), 0);

    EXPECT_EQ(scheduler.stats.total_jobs, 5);
    EXPECT_EQ(scheduler.stats.pending_jobs, 1);
    EXPECT_EQ(scheduler.stats.running_jobs, 1);
    EXPECT_EQ(scheduler.stats.completed_jobs, 1);
    EXPECT_EQ(scheduler.stats.failed_jobs, 1);
}

// TC005: Scheduler Stats Update with NULL
// Expected: Return -1 and log error.
TEST_F(UT002_JobQueue, TC005_StatsNull)
{
    EXPECT_CALL(logger, OnError()).Times(Exactly(1));
    EXPECT_EQ(scheduler_stats_update(nullptr), -1);
}

// TC006: Create and Destroy Shared Scheduler
// Expected: Create returns valid pointer with initialized state; destroy returns 0.
TEST_F(UT002_JobQueue, TC006_SharedSchedulerCreateDestroy)
{
    EXPECT_CALL(logger, OnError()).Times(Exactly(0));

    shared_scheduler_t *scheduler = shm_scheduler_create();
    ASSERT_NE(scheduler, nullptr);
    EXPECT_EQ(scheduler->next_job_id, 1);
    EXPECT_EQ(scheduler->queue.size, 0);
    EXPECT_EQ(scheduler->queue.head, 0);
    EXPECT_EQ(scheduler->queue.tail, 0);

    EXPECT_EQ(shm_scheduler_destroy(scheduler), 0);
}

// TC007: Destroy Shared Scheduler with NULL
// Expected: Return -1 for invalid argument.
TEST_F(UT002_JobQueue, TC007_SharedSchedulerDestroyNull)
{
    EXPECT_CALL(logger, OnError()).Times(Exactly(0));
    EXPECT_EQ(shm_scheduler_destroy(nullptr), -1);
}

// TC008: Push Job to Queue with NULL
// Expected: Return -1 and log error.
TEST_F(UT002_JobQueue, TC008_JobQueuePushFailed)
{
    EXPECT_CALL(logger, OnError()).Times(Exactly(1));
    EXPECT_EQ(job_queue_push(nullptr, nullptr), -1);
}

// TC009: Pop Job from Queue with NULL
// Expected: Return -1 and log error.
TEST_F(UT002_JobQueue, TC009_JobQueuePopFailed)
{
    EXPECT_CALL(logger, OnError()).Times(Exactly(1));
    EXPECT_EQ(job_queue_pop(nullptr, nullptr), -1);
}

// TC010: Push Job when Queue is Full
// Expected: Return -1 and log error.
TEST_F(UT002_JobQueue, TC010_JobQueuePushFull)
{
    job_queue_t queue{};
    job_t job{};
    queue.size = 100;
    EXPECT_CALL(logger, OnError()).Times(Exactly(1));
    EXPECT_EQ(job_queue_push(&queue, &job), -1);
}

// TC006: Convert Job State to String
// Expected: Correct string representation for each job state, and "UNKNOWN" for invalid states.
TEST(UT003_JobState, TC006_ConvertState)
{
    EXPECT_STREQ(job_state_to_string(JOB_PENDING), "PENDING");
    EXPECT_STREQ(job_state_to_string(JOB_RUNNING), "RUNNING");
    EXPECT_STREQ(job_state_to_string(JOB_COMPLETED), "DONE");
    EXPECT_STREQ(job_state_to_string(JOB_FAILED), "FAILED");
    EXPECT_STREQ(job_state_to_string(static_cast<job_state_t>(10)), "UNKNOWN");
}

} // namespace

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
