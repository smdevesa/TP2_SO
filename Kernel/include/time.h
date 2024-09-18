#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();
uint64_t ms_elapsed();

#endif
