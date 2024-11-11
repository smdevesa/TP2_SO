// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboardDriver.h>
#include <syscall_lib.h>
#include <semaphore.h>
#include <scheduler.h>

#define KB_SEM_NAME "os_kb_sem"
#define EOF -1

extern uint8_t _getScancode();
extern void _updateRegisters();
extern uint64_t * _getRegisters();
static char scancodeToAscii(uint8_t scancode);
static void updateFlags(uint8_t scancode);
static int cb_isEmpty();
static char cb_push(char c);
static char cb_pop();
static int cb_isfull();
static void updateRegisters();
static void send_eof_to_stdin();

/*
 * Keycode matrix:
 * in index zero the char without shift, in index 1 the char with shift
 * 0 if the key doesn't have a char representation
 */
static char keycodeMatrix[KEYS_AMOUNT][2] = {
        {0, 0}, {ESC, ESC}, {'1', '!'}, {'2', '@'}, {'3', '#'},
        {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'},
        {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'},
        {'\t', '\t'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'},
        {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'},
        {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, {0, 0},
        {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'},
        {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'},
        {'\'', '\"'}, {'`', '~'}, {0, 0}, {'\\', '|'}, {'z', 'Z'},
        {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'},
        {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, {0, 0}, {0, 0},
        {0, 0}, {' ', ' '}
};

// Circular buffer for keyboard input
#define BUFFER_SIZE 256

typedef struct CircularBuffer {
    char v[BUFFER_SIZE];
    int readIndex;
    int writeIndex;
    int size;
} TCircularBuffer;

static TCircularBuffer buffer = { .readIndex = 0, .writeIndex = 0, .size = 0 };

// Special keys flags
static volatile uint8_t activeShift = 0;
static volatile uint8_t activeCapsLock = 0;
static volatile uint8_t activeCtrl = 0;
static volatile uint8_t end_of_file = 0;

static volatile uint64_t registers[REGS_AMOUNT];

static volatile uint8_t registersFilled = 0;

int initKeyboardDriver() {
    if(semOpen(KB_SEM_NAME, 0) == -1) {
        return -1;
    }
    return 0;
}

void keyboard_handler() {
    uint8_t scancode = _getScancode();
    updateFlags(scancode);
    char ascii = scancodeToAscii(scancode);
    if(activeCtrl) {
        if(ascii == 'r' || ascii == 'R') {
            registersFilled = 1;
            updateRegisters();
        }
        else if(ascii == 'd' || ascii == 'D') {
            sys_write(1, "^D", 2, 0x00FFFFFF);
            send_eof_to_stdin();
        }
        else if(ascii == 'c' || ascii == 'C') {
            sys_write(1, "^C", 2, 0x00FFFFFF);
            kill_process_in_foreground();
        }
    }
    else if (ascii != 0) {
        cb_push(ascii);
        semPost(KB_SEM_NAME);
    }
}

char kb_getchar() {
    update_stdin_waiting(1);
    semWait(KB_SEM_NAME);
    update_stdin_waiting(0);
    return cb_pop();
}

static char scancodeToAscii(uint8_t scancode) {
    char ascii = 0;
    if (scancode < KEYS_AMOUNT) {
        ascii = keycodeMatrix[scancode][activeShift];
        if (activeCapsLock && ascii >= 'a' && ascii <= 'z') {
            ascii -= 32;
        }
    }
    return ascii;
}

static void updateFlags(uint8_t scancode) {
    if (scancode == LCTRL) {
        activeCtrl = 1;
    }
    else if (scancode == LCTRL_RELEASE) {
        activeCtrl = 0;
    }
    else if (scancode == LSHIFT || scancode == RSHIFT) {
        activeShift = 1;
    }
    else if (scancode == (LSHIFT + RELEASE_OFFSET) || scancode == (RSHIFT + RELEASE_OFFSET)) {
        activeShift = 0;
    }
    else if (scancode == CAPSLOCK) {
        activeCapsLock = !activeCapsLock;
    }
}

static int cb_isEmpty() {
    return buffer.readIndex == buffer.writeIndex;
}

static char cb_push(char c) {
    if (cb_isfull()) {
        return 0;
    }
    buffer.v[buffer.writeIndex] = c;
    buffer.writeIndex = (buffer.writeIndex + 1) % BUFFER_SIZE;
    buffer.size++;
    return 1;
}

static int cb_isfull() {
    return buffer.size == BUFFER_SIZE;
}

static char cb_pop() {
    if(cb_isEmpty() && !end_of_file) {
        return 0;
    }

    if(end_of_file) {
        end_of_file = 0;
        return EOF;
    }

    char c = buffer.v[buffer.readIndex];
    buffer.readIndex = (buffer.readIndex + 1) % BUFFER_SIZE;
    buffer.size--;
    return c;
}

void updateRegisters() {
    _updateRegisters();
    uint64_t * r = _getRegisters();
    for(int i = 0; i < REGS_AMOUNT; i++) {
        registers[i] = r[i];
    }
}

uint64_t getRegisters(uint64_t * r) {
    if(!registersFilled) {
        return 0;
    }
    for(int i = 0; i < REGS_AMOUNT; i++) {
        r[i] = registers[i];
    }
    return 1;
}

static void send_eof_to_stdin() {
    end_of_file = 1;
    semPost(KB_SEM_NAME);
}

void release_stdin() {
    semPost(KB_SEM_NAME);
}
