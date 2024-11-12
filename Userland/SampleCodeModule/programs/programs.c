// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <iolib.h>
#include <programs.h>
#include <shell.h>
#include <syscalls.h>

#define EOF -1
#define PS_SECONDARY_COLOR 0x00F5ED51

static void print_ps_header();
static uint8_t is_vowel_or_line_break(char c);

int cat(int argc, char **argv) {
	if (argc != 0) {
		printf("cat: Invalid amount of arguments.\n");
		return -1;
	}

	int c;
	while ((c = getchar()) != EOF) {
		if (c)
			putchar(c);
	}

	return 0;
}

int ps(int argc, char **argv) {
	if (argc != 0) {
		printf("ps: Invalid amount of arguments.\n");
		return -1;
	}

	processInfo_t *process_list = _sys_ps();
	char *status_string[] = {"READY", "BLOCKED", "RUNNING", "TERMINATED"};
	processInfo_t *current = process_list;
	print_ps_header();
	while (current->pid != -1) {
		printf("%d; %s; %d; %d; %d; %s; %x\n", current->pid, current->name,
			   current->parent, current->priority, current->unkillable,
			   status_string[current->status], (uint64_t) current->stackBase);
		current++;
	}
	return 0;
}

int loop(int argc, char **argv) {
	if (argc != 1) {
		printf("loop: Invalid amount of arguments. Usage: loop [ticks]\n");
		return -1;
	}
	int pid = _sys_getpid();
	int ticks = atoi(argv[0]);
	if (ticks < 0) {
		printf("loop: Invalid argument. Ticks must be a positive integer.\n");
		return -1;
	}
	while (1) {
		if (noScreenSpace()) {
			clearScreen();
		}
		printf("%d ", pid);
		_sys_sleep(ticks);
	}
}

int filter(int argc, char **argv) {
	if (argc != 0) {
		printf("filter: Invalid amount of arguments.\n");
		return -1;
	}

	int c;
	while ((c = getchar()) != EOF) {
		if (is_vowel_or_line_break(c))
			putchar(c);
	}

	return 0;
}

int wc(int argc, char **argv) {
	if (argc != 0) {
		printf("wc: Invalid amount of arguments.\n");
		return -1;
	}

	int lines = 0;
	int c;
	while ((c = getchar()) != EOF) {
		if (c == '\n') {
			lines++;
		}
	}

	printf("%d\n", lines);
	return 0;
}

int mem(int argc, char **argv) {
	if (argc != 0) {
		printf("mem: Invalid amount of arguments.\n");
		return -1;
	}

	mem_info_t *mem_info = _sys_get_mem_info();
	printf("Total memory: %d bytes\nUsed memory: %d bytes\nFree memory: %d bytes\n",
		   mem_info->total_mem, mem_info->used_mem, mem_info->free_mem);
	return 0;
}

static uint8_t is_vowel_or_line_break(char c) {
	return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' ||
		   c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == '\n';
}

static void print_ps_header() {
	printStringColor(
		"PID; NAME; PARENT; PRIORITY; UNKILLABLE; STATUS; STACK_BASE\n",
		PS_SECONDARY_COLOR);
}