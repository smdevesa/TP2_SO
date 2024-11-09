#include "../include/time.h"
#include <scheduler.h>

static unsigned long ticks = 0;
typedef struct sleeping {
    uint64_t ticks;
    uint64_t start_tick;
    int32_t pid;
} sleeping_process_t;

static sleeping_process_t sleeping_processes[MAX_PROCESSES];

static void unblock_sleeping_processes();

uint64_t timer_handler(uint64_t rsp) {
	ticks++;
    unblock_sleeping_processes();
    rsp = (uint64_t)schedule((void *)rsp);
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
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (sleeping_processes[i].pid != -1 &&
            sleeping_processes[i].ticks > 0 && (sleeping_processes[i].start_tick + sleeping_processes[i].ticks) <= ticks) {
            unblockProcess(sleeping_processes[i].pid);
            sleeping_processes[i].pid = -1;
        }
    }
}

void remove_sleeping_process(uint32_t pid) {
    sleeping_processes[pid].pid = -1;
}

void sleep(uint64_t sleep_ticks) {
    uint32_t pid = getPid();
    sleeping_processes[pid].ticks = sleep_ticks;
    sleeping_processes[pid].start_tick = ticks;
    sleeping_processes[pid].pid = pid;
    block_process_sleep(pid, 1);
}

uint8_t is_sleeping(uint32_t pid) {
    return sleeping_processes[pid].pid != -1 && (sleeping_processes[pid].start_tick + sleeping_processes[pid].ticks) > ticks;
}