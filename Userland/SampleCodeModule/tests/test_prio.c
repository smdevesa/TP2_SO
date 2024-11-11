// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/test_syscall.h"
#include "../include/test_util.h"
#include <stdint.h>
#include <stdio.h>

#define MINOR_WAIT \
	1000000 // TODO: Change this value to prevent a process from flooding the
			// screen
#define WAIT \
	1000000000 // TODO: Change this value to make the wait long enough to see
			   // theese processes beeing run at least twice
#define WAIT_STR "30000000"

#define TOTAL_PROCESSES 4

#define LOWEST 1
#define MEDIUM_LOW 2
#define MEDIUM_HIGH 3
#define HIGHEST 4

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM_LOW, MEDIUM_HIGH, HIGHEST};

void test_prio() {
	int64_t pids[TOTAL_PROCESSES];
	char *argv[] = {WAIT_STR, NULL};
	int fds[2] = {STDIN, STDOUT};
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++) {
		pids[i] = my_create_process((mainFunction) &endless_loop_print, argv,
									"endless_loop_print", 0, fds);
		if (pids[i] < 0) {
			printf("test_prio: error creating process %d\n", (int) i);
		}
	}

	bussy_wait(WAIT);
	printf("\nCHANGING PRIORITIES...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], prio[i]);

	bussy_wait(WAIT);
	printf("\nBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_block(pids[i]);

	printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], MEDIUM_LOW);

	printf("UNBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_unblock(pids[i]);

	bussy_wait(WAIT);
	printf("\nKILLING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_kill(pids[i]);
}