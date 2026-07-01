# 1. Summary
- MiniScheduler l? project l?p l?ch ch?y l?nh theo m? h?nh daemon-client tr?n Linux:
    - Daemon ch?y n?n, nh?n l?nh qua Unix Domain Socket.
    - Client g?i l?nh "add", "status", "stop".
    - Worker ch?y theo m? h?nh multi-process, d?ng shared memory ?? chia s? h?ng ??i c?ng vi?c  v? tr?ng th?i.

# 2. Overall Architecture
- minisched_daemon (process cha):
  - Kh?i t?o config, log path, shared memory.
  - Fork nhi?u worker theo "num_workers".
  - Listen socket v? x? l? l?nh t? client.

- minisched_client:
  - Parse CLI v? g?i l?nh ??n daemon qua socket.

- Shared memory ("/dev/shm/minisched_job_queue_shm"):
  - job_queue_t (circular queue).
  - history_jobs (l?ch s? job).
  - scheduler_stats_t (th?ng k? tr?ng th?i job).
  - mutex + condition variable v?i PTHREAD_PROCESS_SHARED.

# 3. Directory Structure
- "apps/daemon/main_daemon.cpp": v?ng l?p daemon, x? l? command, shutdown.
- "apps/client/main_client.cpp": CLI client.
- "src/ipc/unix_socket.cpp": socket API, logging, daemonize.
- "src/job/job_queue.cpp": queue, shared memory, stats.
- "src/config/config_loader.cpp": parse file config.
- "config/minisched.conf": c?u h?nh runtime.
- "tests/": unit test gtest/gmock + coverage.

MiniScheduler/
„¥„Ÿ„Ÿ apps                        // Main file.
„ ?? „¥„Ÿ„Ÿ client
„ ?? „ ?? „¤„Ÿ„Ÿ main_client.cpp     // CLI client - parse v? g?i command cho daemon.
„ ?? „¤„Ÿ„Ÿ daemon
„ ??     „¤„Ÿ„Ÿ main_daemon.cpp     // X? l? command, ph?n c?ng worker, respond status cho client.
„¥„Ÿ„Ÿ config
„ ?? „¤„Ÿ„Ÿ minisched.conf          // Ch?a config c?a app.
„¥„Ÿ„Ÿ docs
„¥„Ÿ„Ÿ include
„ ?? „¥„Ÿ„Ÿ config
„ ?? „ ?? „¥„Ÿ„Ÿ config.h            // Config structure and definations.
„ ?? „ ?? „¤„Ÿ„Ÿ config_loader.h     // Parse config file.
„ ?? „¥„Ÿ„Ÿ ipc
„ ?? „ ?? „¥„Ÿ„Ÿ protocol.h
„ ?? „ ?? „¤„Ÿ„Ÿ unix_socket.h       // Unix domain socket.
„ ?? „¤„Ÿ„Ÿ job
„ ??     „¤„Ÿ„Ÿ job.h               // Job's data structure v? job handlings.
„¥„Ÿ„Ÿ logs
„¥„Ÿ„Ÿ minisched_client            // Executable.
„¥„Ÿ„Ÿ minisched_daemon
„¥„Ÿ„Ÿ src
„ ?? „¥„Ÿ„Ÿ config
„ ?? „ ?? „¤„Ÿ„Ÿ config_loader.cpp
„ ?? „¥„Ÿ„Ÿ ipc
„ ?? „ ?? „¤„Ÿ„Ÿ unix_socket.cpp
„ ?? „¤„Ÿ„Ÿ job
„ ??     „¤„Ÿ„Ÿ job_queue.cpp
„¤„Ÿ„Ÿ tests
„    „¥„Ÿ„Ÿ Makefile                // Build rules for testing.
„    „¥„Ÿ„Ÿ main.cpp
„    „¤„Ÿ„Ÿ mock.h
„¥„Ÿ„Ÿ Makefile                    // Build rules.
„¤„Ÿ„Ÿ README.md

# 4. Build
- Project x?y d?ng tr?n m?i tr??ng Linux.
- Build:
    make all
- Executable:
    ./minisched_daemon
    ./minisched_client

# 5. Configuration
File: config/minisched.conf

num_workers=4                       // Max worker processes.
socket_path=/tmp/minisched.sock     // Path c?a object Unix domain socket.

# 6. Usage Instructions
# 6.1 Start daemon
./minisched_daemon

## 6.2 G?i job
./minisched_client add "ls -l"
./minisched_client add "sleep 3"

## 6.3 Xem tr?ng th?i + th?ng k?
./minisched_client status

## 6.4 D?ng daemon graceful
./minisched_client stop
or pkill -15 -f minisched_daemon

# 7. Logging
- Log output: "logs/minisched_*.log"
- Log output command: "logs/minisched_*_cmd.log"

# 8. Unit test (gtest/gmock)
Project ?? c? test scaffold trong "tests/":
- "config_loader_test.cpp"
- "job_queue_test.cpp"
- mock logger trong "tests/mocks/"

## 8.1 Ch?y test b?ng Makefile
cd tests
make test

## 8.2 Ch?y coverage b?ng Makefile
cd tests
make coverage

K?t qu? coverage HTML: "tests/coverage.html"

## 8.3 Ch?y test b?ng CMake
cd tests
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
cmake --build build --target coverage

Coverage HTML (CMake target): "tests/build/coverage.html"

# 9. Debug command
- Xem process:
ps -ef
ps aux | grep minisched

- Kill process:
kill -9 <pid>               // Force kill
kill -15 <pid>              // Graceful kill
pkill -f minisched_daemon   // Kill all processes

- Xem log realtime:
tail -f logs/minisched_*.log

# 10. Notes
- Daemon d?ng signal handling cho "SIGINT"/"SIGTERM" ?? b?t signal.
- Log path s? d?ng shared memory ?? process daemon, client, worker c? th? ghi log v?o c?ng file.
- Schedule job s? d?ng shared memory k?t h?p mutex, condition ?? ghi, ??c job t? job queue.
- L?c shutdown, daemon process d?ng waitpid ?? d?n d?p pid table sau khi c?c process con exit.