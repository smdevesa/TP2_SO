#include <keyboardDriver.h>
#include <stddef.h>

extern uint8_t _getScancode();
static char scancodeToAscii(uint8_t scancode);
static void updateFlags(uint8_t scancode);
static int cb_isEmpty();
static char cb_push(char c);
static char cb_pop();
static int cb_isfull();

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

// User CTRL + R function
static void (*ctrlRAction)(void) = NULL;

void keyboard_handler() {
    uint8_t scancode = _getScancode();
    updateFlags(scancode);
    char ascii = scancodeToAscii(scancode);
    if(activeCtrl && (ascii == 'r' || ascii == 'R')) {
        if (ctrlRAction != NULL) {
            ctrlRAction();
        }
    }
    else if (ascii != 0) {
        cb_push(ascii);
    }
}

char kb_getchar() {
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
    // If the buffer is full, don't push anything
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
    // If the buffer is empty, don't pop anything
    if (cb_isEmpty()) {
        return 0;
    }

    char c = buffer.v[buffer.readIndex];
    buffer.readIndex = (buffer.readIndex + 1) % BUFFER_SIZE;
    buffer.size--;
    return c;
}

void setCtrlRAction(void (*action)(void)) {
    if(action != NULL) {
        ctrlRAction = action;
    }
}