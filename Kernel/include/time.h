#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

uint64_t timer_handler(uint64_t rsp);
int ticks_elapsed();
int seconds_elapsed();
uint64_t ms_elapsed();

#endif
