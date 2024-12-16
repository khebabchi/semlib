# Compiler and flags
CC = gcc
CFLAGS = -Wall -g  # Add debugging and warnings

# Source files
SRCS = main.c semaphore.c int_shm.c char_shm.c  # Add all source files
OBJS = $(SRCS:%.c=out/%.o)  # Convert source files to object files in the out directory

# Output binary
TARGET = out/main

# Default target
all: out $(TARGET)

# Ensure the output directory exists
out:
	mkdir -p out

# Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compile source files into object files in the out directory
out/%.o: %.c | out
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf out
