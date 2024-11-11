#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

uint64_t timer_handler(uint64_t rsp);
int ticks_elapsed();
int seconds_elapsed();
uint64_t ms_elapsed();
void init_sleeping_processes();
void remove_sleeping_process(uint32_t pid);
void sleep(uint64_t sleep_ticks);

#endif
