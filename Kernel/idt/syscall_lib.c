// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <audioDriver.h>
#include <font.h>
#include <interrupts.h>
#include <keyboardDriver.h>
#include <memory_manager.h>
#include <pipes.h>
#include <rtc.h>
#include <scheduler.h>
#include <semaphore.h>
#include <syscall_lib.h>
#include <time.h>
#include <videoDriver.h>

#define TAB_SIZE 4

#define WHITE 0x00FFFFFF
#define BLACK 0x00000000
#define DEFAULT_BG_COLOR 0x00000000

static uint16_t print_x = 0;
static uint16_t print_y = 0;

uint32_t bg_color = DEFAULT_BG_COLOR;

static int print_special_cases(char c);
static int print_buffer(const char *buffer, int count, uint32_t color);

int64_t sys_read(int fd, char *buffer, int count) {
	if (fd < 0 || count <= 0)
		return -1;

	if (fd == STDIN) {
		int fds[2];
		getCurrentFDs(fds);
		if (fds[0] != STDIN) {
			return readPipe(fds[0], buffer, count);
		}
		else {
			for (int i = 0; i < count; i++) {
				char c = kb_getchar();
				if (c == 0) {
					return i;
				}
				buffer[i] = c;
			}
			return count;
		}
	}
	// if the file descriptor is not STDIN, it must be a pipe
	return readPipe(fd, buffer, count);
}

int64_t sys_write(int fd, const char *buffer, int count, uint32_t color) {
	if (fd < 1 || count <= 0)
		return -1;
	if (fd == STDOUT) {
		int fds[2];
		getCurrentFDs(fds);
		if (fds[1] != STDOUT) {
			return writePipe(fds[1], buffer, count);
		}
		return print_buffer(buffer, count, color);
	}
	// if the file descriptor is not STDOUT, it must be a pipe
	return writePipe(fd, buffer, count);
}

static int print_buffer(const char *buffer, int count, uint32_t color) {
	for (int i = 0; i < count; i++) {
		if (buffer[i] != ESC) {
			// Check if the character fits in the screen
			if ((print_x + getFontWidth() * getScale()) > getScreenWidth()) {
				print_x = 0;
				print_y += getFontHeight() * getScale();
			}

			if ((print_y + getFontHeight() * getScale()) > getScreenHeight()) {
				// No more space in the screen, return the number of characters written
				return i;
			}

			// Check if the character is a special case
			if (!print_special_cases(buffer[i])) {
				drawChar(buffer[i], color, bg_color, print_x, print_y);
				print_x += getFontWidth() * getScale();
			}
		}
	}
	return count;
}

static int print_special_cases(char c) {
	switch (c) {
		case '\n':
			print_x = 0;
			print_y += getFontHeight() * getScale();
			return 1;
		case '\t':
			print_x += getFontWidth() * getScale() * TAB_SIZE;
			return 1;
		case '\b':
			if (print_x > 0) {
				print_x -= getFontWidth() * getScale();
			}
			else if (print_y > 0) {
				print_y -= getFontHeight() * getScale();
				print_x = getScreenWidth() - getFontWidth() * getScale();
				// Align the cursor to the previous line
				print_x -= print_x % (getFontWidth() * getScale());
			}
			drawChar(' ', BLACK, bg_color, print_x, print_y);
			return 1;
		default:
			return 0;
	}
}

int64_t sys_draw_rectangle(uint32_t hex_color, uint64_t x, uint64_t y,
						   uint64_t width, uint64_t height) {
	return drawRectangle(hex_color, x, y, width, height);
}

int64_t sys_get_coords() {
	// Return the y in the high 32 bits and the x in the low 32 bits
	return ((uint64_t) print_y << 32) | print_x;
}

int64_t sys_clear_screen() {
	drawRectangle(bg_color, 0, 0, getScreenWidth(), getScreenHeight());
	print_x = 0;
	print_y = 0;
	return 0;
}

int64_t sys_get_screen_info() {
	// return the width in the high 32 bits and the height in the low 32 bits
	return ((uint64_t) getScreenWidth() << 32) | getScreenHeight();
}

int64_t sys_get_font_info() {
	// return the width in the high 32 bits and the height in the low 32 bits
	return ((uint64_t) (getFontWidth() * getScale()) << 32) |
		   (getFontHeight() * getScale());
}

int64_t sys_get_time(uint64_t arg) {
	return getTime(arg);
}

int64_t sys_set_font_scale(uint64_t scale) {
	return setScale(scale);
}

int64_t sys_get_registers(uint64_t *r) {
	return getRegisters(r);
}

int64_t sys_sleep(uint64_t ticks) {
	sleep(ticks);
	return 1;
}

int64_t sys_play_sound(uint64_t f, uint64_t millis) {
	playSound(f);
	sys_sleep(millis);
	stopSound();
	return 1;
}

int64_t sys_set_bg_color(uint32_t color) {
	bg_color = color;
	return 1;
}

int64_t sys_get_bg_color() {
	return bg_color;
}

int64_t sys_yield() {
	yield();
	return 1;
}

int64_t sys_getpid() {
	return getPid();
}

int64_t sys_create_process(uint64_t main, char **argv, char *name,
						   uint8_t unkillable, int *fileDescriptors) {
	return addProcess((mainFunction) main, argv, name, unkillable,
					  fileDescriptors);
}

int64_t sys_block_process(uint16_t pid) {
	return blockProcess(pid);
}

int64_t sys_unblock_process(uint16_t pid) {
	return unblockProcess(pid);
}

int64_t sys_change_priority(uint16_t pid, uint8_t newPriority) {
	return changePriority(pid, newPriority);
}

int64_t sys_kill_process(uint16_t pid) {
	return killProcess(pid);
}

int64_t sys_exit(int64_t retValue) {
	my_exit(retValue);
	return 1;
}

int64_t sys_malloc(uint64_t size) {
	return (uint64_t) my_malloc(size);
}

int64_t sys_free(uint64_t ptr) {
	my_free((void *) ptr);
	return 1;
}

int64_t sys_ps() {
	return (uint64_t) ps();
}

int64_t sys_sem_open(char *name, uint64_t initialValue) {
	return semOpen(name, initialValue);
}

int64_t sys_sem_close(char *name) {
	return semClose(name);
}

int64_t sys_sem_wait(char *name) {
	return semWait(name);
}

int64_t sys_sem_post(char *name) {
	return semPost(name);
}

int64_t sys_waitpid(uint16_t pid) {
	return waitPid(pid);
}

int64_t sys_create_pipe(int fds[2]) {
	return createPipe(fds);
}

int64_t sys_destroy_pipe(int writeFd) {
	destroyPipe(writeFd);
	return 1;
}

int64_t sys_get_mem_info() {
	return (uint64_t) mem_dump();
}
