#include "../include/time.h"
#include <scheduler.h>

static unsigned long ticks = 0;

uint64_t timer_handler(uint64_t rsp) {
	ticks++;
    uint64_t newRSP = (uint64_t) schedule((void *) rsp);
    return newRSP;
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