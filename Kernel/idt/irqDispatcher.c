#include <time.h>
#include <stdint.h>
#include <keyboardDriver.h>

#define TIMERTICK 0
#define KEYBOARD 1

static void int_20(uint64_t rsp);
static void int_21();

void * irqDispatcher(uint64_t irq, uint64_t rsp) {
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

void int_20(uint64_t rsp) {
	timer_handler(rsp);
}

void int_21() {
    keyboard_handler();
}
