
#ifndef TPE_ARQ_KEYBOARDDRIVER_H
#define TPE_ARQ_KEYBOARDDRIVER_H

#include <stdint.h>

#define ESC 27
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define LCTRL 0x1D
#define LCTRL_RELEASE 0x9D
#define CAPSLOCK 0x3A
#define RELEASE_OFFSET 0x80
#define KEYS_AMOUNT 59

int initKeyboardDriver();

/**
 * @brief Hardware interrupt handler for the keyboard.
 *
 */
void keyboard_handler();

/**
 * @brief Returns a character from the keyboard buffer
 * or 0 if the buffer is empty.
 */
char kb_getchar();

/**
 * @brief Fills the registers array with the current values of the registers.
 * You must update the registers with CTRL + R before calling this function.
 * @param r: the array to fill.
 * @return 1 if the registers were filled, 0 otherwise.
 */
uint64_t getRegisters(uint64_t * r);

void release_stdin();

#endif
