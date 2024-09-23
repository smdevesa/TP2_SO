CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =

# Source files
SRCS = mmTest.c naive_mm.c test_util.c
# Header files
HDRS = test_util.h memory_manager.h
# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = mmTest

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean