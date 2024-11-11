#ifndef TPE_ARQ_SYSCALL_LIB_H
#define TPE_ARQ_SYSCALL_LIB_H

#include <stdint.h>

#define REGS_AMOUNT 17
#define STDIN 0
#define STDOUT 1

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

/**
 * @brief Reads a string from the standard input.
 * @param buffer: the buffer to store the string.
 * @param count: the number of chars to read.
 * @return the number of chars read.
 */
int64_t sys_read(int fd, char *buffer, int count);

/**
 * @brief Writes a string to the standard output.
 * @param buffer: the string to write.
 * @param count: the number of chars to write.
 * @param color: the color of the string in hexadecimal. Usage: 0x00RRGGBB.
 * @return the number of chars written.
 */
int64_t sys_write(int fd, const char *buffer, int count, uint32_t color);

/**
 * @brief Draws a rectangle in the screen on the given coordinates.
 * @param hex_color: the color of the rectangle in hexadecimal. Usage:
 * 0x00RRGGBB.
 * @param x: the x coordinate of the rectangle.
 * @param y: the y coordinate of the rectangle.
 * @param width: the width of the rectangle.
 * @param height: the height of the rectangle.
 * @return 0 if the rectangle was drawn successfully, 1 if the rectangle was out
 * of bounds.
 */
int64_t sys_draw_rectangle(uint32_t hex_color, uint64_t x, uint64_t y,
						   uint64_t width, uint64_t height);

/**
 * @brief Clears the screen.
 * @return status of the operation.
 */
int64_t sys_clear_screen();

/**
 * @brief Returns the current cursor position.
 * @return the current cursor y position in the higher 32 bits and the x
 * position in the lower 32 bits.
 */
int64_t sys_get_coords();

/**
 * @brief Returns the screen width in the high 32 bits and the screen height in
 * the low 32 bits.
 * @return screen information.
 */
int64_t sys_get_screen_info();

/**
 * @brief Returns the font width in the high 32 bits and the font height in the
 * low 32 bits.
 * @return font information.
 */
int64_t sys_get_font_info();

/**
 * @brief Returns the time data of the current time read from the RTC.
 * @param rdi: additional parameter for time request.
 * @return current time.
 */
int64_t sys_get_time(uint64_t rdi);

/**
 * @brief Sets the font scale. Supported values are 1, 2, and 3.
 * @param scale: the scale to set.
 * @return status of the operation.
 */
int64_t sys_set_font_scale(uint64_t scale);

/**
 * @brief Fills the registers array with the current values of the registers.
 * Registers must be updated with CTRL + R before calling this function.
 * @param r: the array to fill with the registers. Be sure of having at least
 * REGS_AMOUNT elements.
 * @return 0 if the registers were filled successfully, 1 if the registers were
 * not filled.
 */
int64_t sys_get_registers(uint64_t *r);

/**
 * @brief Sleeps the current process for the given amount of milliseconds.
 * @param millis: the amount of milliseconds to sleep.
 * @return status of the operation.
 */
int64_t sys_sleep(uint64_t millis);

/**
 * @brief Plays a sound with the given frequency.
 * @param f: frequency of the sound.
 * @param millis: the amount of milliseconds to last.
 * @return status of the operation.
 */
int64_t sys_play_sound(uint64_t f, uint64_t millis);

/**
 * @brief Sets the background color of the screen.
 * @param color: the color to set in hexadecimal. Usage: 0x00RRGGBB.
 * @return status of the operation.
 */
int64_t sys_set_bg_color(uint32_t color);

/**
 * @brief Returns the current background color of the screen.
 * @return the current background color in hexadecimal. Usage: 0x00RRGGBB.
 */
int64_t sys_get_bg_color();

/**
 * @brief Yields the current process.
 * @return status of the operation.
 */
int64_t sys_yield();

/**
 * @brief Returns the PID of the current process.
 * @return the PID of the current process.
 */
int64_t sys_getpid();

/**
 * @brief Creates a new process.
 * @param main: the main function of the process.
 * @param argv: the arguments of the process.
 * @param name: the name of the process.
 * @param unkillable: if the process is unkillable.
 * @param fileDescriptors: the file descriptors of the process. On index 0 is
 * the input file descriptor and on index 1 is the output file descriptor.
 * @return 0 if the process was created successfully, -1 otherwise.
 */
int64_t sys_create_process(uint64_t main, char **argv, char *name,
						   uint8_t unkillable, int *file_descriptors);

/**
 * @brief Blocks the process with the given PID.
 * @param pid: the PID of the process to block.
 * @return 0 if the process was blocked successfully, -1 if the process was not
 * blocked.
 */
int64_t sys_block_process(uint16_t pid);

/**
 * @brief Unblocks the process with the given PID.
 * @param pid: the PID of the process to unblock.
 * @return 0 if the process was unblocked successfully, -1 if the process was
 * not unblocked.
 */
int64_t sys_unblock_process(uint16_t pid);

/**
 * @brief Changes the priority of the process with the given PID.
 * @param pid: the PID of the process to change the priority.
 * @param new_priority: the new priority of the process.
 * @return 0 if the priority was changed successfully, -1 if the priority was
 * not changed.
 */
int64_t sys_change_priority(uint16_t pid, uint8_t new_priority);

/**
 * @brief Kills the process with the given PID.
 * @param pid: the PID of the process to kill.
 * @param ret_value: the return value of the process.
 * @return 0 if the process was killed successfully, -1 if the process was not
 * killed.
 */
int64_t sys_kill_process(uint16_t pid);

/**
 * @brief Ends the execution of the caller process.
 * @param ret_value: the return value of the caller process.
 * @return status of the operation.
 */
int64_t sys_exit(int64_t ret_value);

/**
 * @brief Allocates memory.
 * @param size: the size of the memory to allocate.
 * @return the address of the allocated memory.
 */
int64_t sys_malloc(uint64_t size);

/**
 * @brief Frees memory.
 * @param ptr: the address of the memory to free.
 * @return status of the operation.
 */
int64_t sys_free(uint64_t ptr);

/**
 * @brief Returns the process list terminated with a process with pid == -1.
 * @return the list of processes.
 */
int64_t sys_ps();

/**
 * @brief Opens a new named semaphore.
 * @param name: the name of the semaphore.
 * @param initial_value: the initial value of the semaphore.
 * @return 0 if the semaphore was opened successfully, -1 if the semaphore was
 * not opened.
 */
int64_t sys_sem_open(char *name, uint64_t initial_value);

/**
 * @brief Closes a named semaphore.
 * @param name: the name of the semaphore.
 * @return 0 if the semaphore was closed successfully.
 */
int64_t sys_sem_close(char *name);

/**
 * @brief Waits for a named semaphore.
 * @param name: the name of the semaphore.
 * @return 0 if the semaphore was waited, -1 if the semaphore was not waited.
 */
int64_t sys_sem_wait(char *name);

/**
 * @brief Posts a named semaphore.
 * @param name: the name of the semaphore.
 * @return 0 if the semaphore was posted, -1 if the semaphore was not posted.
 */
int64_t sys_sem_post(char *name);

/**
 * @brief Waits for a process to finish.
 * @param pid: the PID of the process to wait.
 * @return the return value of the process.
 */
int64_t sys_waitpid(uint16_t pid);

/**
 * @brief Creates a new pipe.
 * @param fds: the file descriptors of the pipe.
 * @return 0 if the pipe was created successfully, -1 if the pipe was not
 * created.
 */
int64_t sys_create_pipe(int fds[2]);

/**
 * @brief Destroys a pipe.
 * @param write_fd: the write file descriptor of the pipe.
 * @return status of the operation.
 */
int64_t sys_destroy_pipe(int write_fd);

/**
 * @return the memory information of the system.
 */
int64_t sys_get_mem_info();

#endif
