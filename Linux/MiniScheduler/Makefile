#=================================================
# Makefile
#=================================================

# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -O2

# Source files
INCS = include
SRCS = src

# Target executable
all: minisched_daemon minisched_client

# General pattern for executables:
# target_executable: dependencies(if any of these changed, the target will be recompiled)
# <TAB> compiler flags -o target_executable source_files(input file)

# For Daemon executable
minisched_daemon: $(SRCS)/UnixSocket.cpp $(SRCS)/main_daemon.cpp $(INCS)/UnixSocket.h $(INCS)/protocol.h
	$(CC) $(CFLAGS) -o minisched_daemon $(SRCS)/UnixSocket.cpp $(SRCS)/main_daemon.cpp

# For Client executable
minisched_client: $(SRCS)/UnixSocket.cpp $(SRCS)/main_client.cpp $(INCS)/UnixSocket.h $(INCS)/protocol.h
	$(CC) $(CFLAGS) -o minisched_client $(SRCS)/UnixSocket.cpp $(SRCS)/main_client.cpp

# Clean up the executables
clean:
	rm -f minisched_daemon minisched_client