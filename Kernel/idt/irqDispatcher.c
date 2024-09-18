#include <time.h>
#include <stdint.h>
#include <keyboardDriver.h>

#define TIMERTICK 0
#define KEYBOARD 1

static void int_20();
static void int_21();

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case TIMERTICK:
			int_20();
			break;
        case KEYBOARD:
            int_21();
            break;
	}
	return;
}

void int_20() {
	timer_handler();
}

void int_21() {
    keyboard_handler();
}
