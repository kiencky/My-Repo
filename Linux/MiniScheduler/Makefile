#=================================================
# Makefile
#=================================================

# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -O2
PTHREAD = -pthread

# Source files
INC = include
SRC = src

# Dependency.
ALL_INCS = $(shell find $(INC) -name "*.h")
IPC_SRCS   = $(SRC)/ipc/unix_socket.cpp
CONFIG_SRCS = $(SRC)/config/config_loader.cpp
JOB_SRCS = $(SRC)/job/job_queue.cpp

# Target executable
all: minisched_daemon minisched_client

# General pattern for executables:
# target_executable: dependencies(if any of these changed, the target will be recompiled)
# <TAB> compiler flags -o target_executable source_files(input file)

# For Daemon executable
DAEMON_SRCS = $(IPC_SRCS) $(CONFIG_SRCS) $(JOB_SRCS) apps/daemon/main_daemon.cpp
minisched_daemon: $(DAEMON_SRCS) $(ALL_INCS)
	$(CC) $(CFLAGS) $(PTHREAD) -o minisched_daemon $(DAEMON_SRCS)

# For Client executable
CLIENT_SRCS = $(IPC_SRCS) $(CONFIG_SRCS) apps/client/main_client.cpp
minisched_client: $(CLIENT_SRCS) $(ALL_INCS)
	$(CC) $(CFLAGS) $(PTHREAD) -o minisched_client $(CLIENT_SRCS)

# Clean up the executables
clean:
	rm -f minisched_daemon minisched_client