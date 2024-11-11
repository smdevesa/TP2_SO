// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/time.h"
#include <scheduler.h>
#include <stdint.h>

typedef struct sleeping {
	uint64_t end_tick;
	int32_t pid;
} sleeping_process_t;

static sleeping_process_t sleeping_processes[MAX_PROCESSES];
static uint64_t ticks = 0;
static uint64_t next_tick_check = UINT64_MAX;

static void unblock_sleeping_processes();

uint64_t timer_handler(uint64_t rsp) {
	ticks++;
	unblock_sleeping_processes();
	rsp = (uint64_t) schedule((void *) rsp);
	return rsp;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

uint64_t ms_elapsed() {
	return ticks * 55;
}

void init_sleeping_processes() {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		sleeping_processes[i].pid = -1;
	}
}

static void unblock_sleeping_processes() {
	if (ticks < next_tick_check)
		return;

	next_tick_check = UINT64_MAX;
	for (int i = 0; i < MAX_PROCESSES; i++) {
		if (sleeping_processes[i].pid != -1 &&
			sleeping_processes[i].end_tick <= ticks) {
			unblockProcess(sleeping_processes[i].pid);
			sleeping_processes[i].pid = -1;
		}
		else if (sleeping_processes[i].pid != -1 &&
				 sleeping_processes[i].end_tick < next_tick_check) {
			next_tick_check = sleeping_processes[i].end_tick;
		}
	}
}

void remove_sleeping_process(uint32_t pid) {
	sleeping_processes[pid].pid = -1;
}

void sleep(uint64_t sleep_ticks) {
	uint32_t pid = getPid();
	sleeping_processes[pid].end_tick = ticks + sleep_ticks;
	sleeping_processes[pid].pid = pid;
	if (sleeping_processes[pid].end_tick < next_tick_check)
		next_tick_check = sleeping_processes[pid].end_tick;
	block_process_sleep(pid, 1);
}
