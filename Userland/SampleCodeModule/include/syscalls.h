#ifndef TPE_ARQ_SYSCALLS_H
#define TPE_ARQ_SYSCALLS_H

#include <stdint.h>

/*
 * Arguments used to read the RTC.
 */
#define SECONDS_ARG 0x00
#define MINUTES_ARG 0x02
#define HOURS_ARG 0x04
#define DAY_ARG 0x07
#define MONTH_ARG 0x08
#define YEAR_ARG 0x09

/*
 * File descriptors.
 */
#define STDIN_FD 0x00
#define STDOUT_FD 0x01

/*
 * getRegisters data
 */
#define REGS_AMOUNT 17

#define MIN_PRIORITY 1
#define MAX_PRIORITY 4

enum registers_idx {
	RAX_IDX = 0,
	RBX_IDX,
	RCX_IDX,
	RDX_IDX,
	RSI_IDX,
	RDI_IDX,
	RBP_IDX,
	R8_IDX,
	R9_IDX,
	R10_IDX,
	R11_IDX,
	R12_IDX,
	R13_IDX,
	R14_IDX,
	R15_IDX,
	RIP_IDX,
	RSP_IDX
};

typedef int (*mainFunction)(int argc, char **argv);

#define MAX_NAME_LENGTH 256

typedef enum status { READY = 0,
					  BLOCKED,
					  RUNNING,
					  TERMINATED } processStatus_t;

typedef struct processInfo {
	int16_t pid;
	int16_t parent;
	char name[MAX_NAME_LENGTH];
	uint8_t priority;
	uint8_t unkillable;
	void *stackBase;
	processStatus_t status;
} processInfo_t;

typedef struct mem_info {
	uint64_t total_mem;
	uint64_t used_mem;
	uint64_t free_mem;
} mem_info_t;

/**
 * @brief Reads a string from the file descriptor.
 * @param fd file descriptor
 * @param buffer the buffer to store the string.
 * @param count the number of bytes to read.
 * @return the number of bytes read.
 */
uint64_t _sys_read(int fd, char *buffer, int count);

/**
 * @brief Writes a string to the file descriptor.
 * @param fd file descriptor
 * @param buffer the string to write.
 * @param count the number of bytes to write.
 * @param color the color of the string in hexadecimal. Usage: 0x00RRGGBB.
 * @return the number of bytes written.
 */
uint64_t _sys_write(int fd, const char *buffer, int count, uint32_t color);

/**
 * @brief Draws a rectangle in the screen on the given coordinates.
 * @param hexColor the color of the rectangle in hexadecimal. Usage: 0x00RRGGBB.
 * @param x top left corner x coordinate of the rectangle.
 * @param y top left corner y coordinate of the rectangle.
 * @param width width of the rectangle in pixels.
 * @param height height of the rectangle in pixels.
 * @return 0 if the rectangle was drawn successfully, 1 if the rectangle was out
 * of bounds.
 */
uint64_t _sys_drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y,
							uint64_t width, uint64_t height);

/**
 * @brief Clears the screen and resets the cursor to the origin of coordinates.
 */
uint64_t _sys_clearScreen();

/**
 * @brief Returns the current cursor position.
 * @return the current cursor y position in the higher 32 bits and the x
 * position in the lower 32 bits.
 */
uint64_t _sys_getCoords();

/**
 * @brief Returns the screen width in the high 32 bits and the screen height in
 * the low 32 bits.
 */
uint64_t _sys_getScreenInfo();

/**
 * @brief Returns the font width in the high 32 bits and the font height in the
 * low 32 bits.
 */
uint64_t _sys_getFontInfo();

/**
 * @brief Returns the data read from the RTC.
 * @param arg the data to read. (Check defines above)
 */
uint64_t _sys_getTime(uint64_t arg);

/**
 * @brief Sets the font scale. Supported values are 1, 2 and 3.
 * @param scale the scale to set.
 */
uint64_t _sys_setFontScale(uint64_t scale);

/**
 * @brief Fills the registers array with the current values of the registers.
 * Registers need to be updated using CTRL + R before calling this function.
 * @param regs the buffer to store the registers. Must be at least 17 elements.
 * @return 1 if the registers were written successfully, 0 otherwise.
 */
uint64_t _sys_getRegisters(uint64_t *regs);

/**
 * @brief Sleeps the current process for the given amount of milliseconds.
 * @param millis the amount of milliseconds to sleep.
 */
uint64_t _sys_sleep(uint64_t millis);

/**
 * @brief Plays a sound with the given frequency.
 * @param f: frequency of the sound.
 * @param millis: the amount of milliseconds to last.
 */
uint64_t _sys_playSound(uint64_t f, uint64_t millis);

/**
 * @brief Returns the current process id.
 */
uint64_t _sys_getpid();

/**
 * @brief Gives the CPU to the next process.
 */
uint64_t _sys_yield();

/**
 * @brief Creates a new process.
 * @param main the main function of the process.
 * @param argv the arguments of the process.
 * @param name the name of the process.
 * @param unkillable if the process is unkillable.
 * @param fileDescriptors the file descriptors of the process. The first element
 * is the input file descriptor and the second element is the output file
 * descriptor.
 * @return the PID of the new process.
 */
int64_t _sys_createProcess(mainFunction main, char **argv, char *name,
						   uint8_t unkillable, int *fileDescriptors);

/**
 * @brief Kills a process.
 * @param pid the process id.
 * @return 0 if the process was killed successfully, -1 otherwise.
 */
int64_t _sys_kill(uint64_t pid);

/**
 * @brief Blocks a process.
 * @param pid the process id.
 * @return 0 if the process was blocked successfully, -1 otherwise.
 */
int64_t _sys_block(uint64_t pid);

/**
 * @brief Unblocks a process.
 * @param pid the process id.
 * @return 0 if the process was unblocked successfully, -1 otherwise.
 */
int64_t _sys_unblock(uint64_t pid);

/**
 * @brief Changes the priority of a process.
 * @param pid the process id.
 * @param newPriority the new priority.
 * @return 0 if the priority was changed successfully, -1 otherwise.
 */
int64_t _sys_changePriority(uint64_t pid, uint64_t newPriority);

/**
 * @brief Ends the execution of the caller process.
 * @param retValue
 */
int64_t _sys_exit(int64_t retValue);

/**
 * @brief Asks the memory manager for a block of memory.
 * @param size: the size of the block.
 * @return the address of the block or NULL if there was an error.
 */
void *_sys_malloc(uint64_t size);

/**
 * @brief Frees a block of memory.
 * @param ptr: the address of the block.
 */
void _sys_free(void *ptr);

/**
 * @brief Returns the process list terminated with a process with pid == -1.
 */
processInfo_t *_sys_ps();

/**
 * @brief Opens a semaphore.
 * @param name: the name of the semaphore.
 * @param initialValue: the initial value of the semaphore.
 * @return 0 if the semaphore was opened successfully, -1 otherwise.
 */
int64_t _sys_semOpen(char *name, uint64_t initialValue);

/**
 * @brief Closes a semaphore.
 * @param name: the name of the semaphore.
 * @return 0 if the semaphore was closed successfully, -1 otherwise.
 */
int64_t _sys_semClose(char *name);

/**
 * @brief Waits for a semaphore.
 * @param name: the name of the semaphore.
 * @return 0 if the semaphore was waited
 *        -1 if the semaphore was not waited
 */
int64_t _sys_semWait(char *name);

/**
 * @brief Posts a semaphore.
 * @param name: the name of the semaphore.
 * @return 0 if the semaphore was posted
 *        -1 if the semaphore was not posted
 */
int64_t _sys_semPost(char *name);

/**
 * @brief Waits for a process to finish.
 * @param pid the process id.
 * @return the return value of the process.
 */
int64_t _sys_waitpid(uint32_t pid);

/**
 * @brief Creates a pipe.
 * @param fds the file descriptors of the pipe.
 * @return 0 if the pipe was created successfully, -1 otherwise.
 */
int64_t _sys_create_pipe(int fds[2]);

/**
 * @brief Destroys a pipe.
 * @param writeFd the write file descriptor of the pipe.
 * @return 0 if the pipe was destroyed successfully, -1 otherwise.
 */
int64_t _sys_destroy_pipe(int writeFd);

/**
 * @brief Returns the memory information.
 * @return a struct with the memory information.
 */
mem_info_t *_sys_get_mem_info();

#endif
