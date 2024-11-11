// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <stdint.h>
#include <keyboardDriver.h>

#define TIMERTICK 0
#define KEYBOARD 1

static uint64_t int_20(uint64_t rsp);
static void int_21();

uint64_t irqDispatcher(uint64_t irq, uint64_t rsp) {
	switch (irq) {
		case TIMERTICK:
			rsp = int_20(rsp);
			break;
        case KEYBOARD:
            int_21();
            break;
	}
	return rsp;
}

uint64_t int_20(uint64_t rsp) {
	rsp = timer_handler(rsp);
    return rsp;
}

void int_21() {
    keyboard_handler();
}
