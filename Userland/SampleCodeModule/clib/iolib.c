#include <iolib.h>
#include <syscalls.h>
#include <stdarg.h>
#include <color.h>
#include <stringutils.h>

#define PRINTF_BUFFER_SIZE 100
#define SCANF_BUFFER_SIZE 100

char getchar() {
    char c;
    _sys_read(STDIN, &c, 1);
    return c;
}

static int readToBlank(char * str, int index) {
    int readBytes = 0;
    for(int i=index; str[i] != 0 && str[i] != '\n' && str[i] != ' ' && str[i] != '\t'; i++) {
        readBytes++;
    }
    return readBytes;
}

void putcharColor(char c, uint32_t color) {
    _sys_write(STDOUT, &c, 1, color);
}

void printStringColor(const char * str, uint32_t color) {
    for(int i=0; str[i] != 0; i++) {
        putcharColor(str[i], color);
    }
}

void putchar(char c) {
    putcharColor(c, WHITE);
}

char * fgets(char *buffer, size_t size) {
    int readBytes = 0;
    char c;
    while(readBytes < (size-1) && (((c = getchar()) != '\n'))) {
        buffer[readBytes++] = c;
    }
    buffer[size-1] = 0;
    return buffer;
}

int atoi(const char *str) {
    int res = 0;
    for (int i = 0; str[i] != 0 && str[i] <= '9' && str[i] >= '0'; i++) {
        res = res * 10 + str[i] - '0';
    }
    return res;
}

char *itoa(int num, char *str) {
    int i = 0;
    if (num < 0) {
        str[i++] = '-';
        num = -num;
    } else if (num == 0) {
        str[i++] = '0';
    }

    while (num != 0) {
        str[i++] = num % 10 + '0';
        num /= 10;
    }

    str[i] = '\0';
    reverse(str + (str[0] == '-' ? 1 : 0));
    return str;
}

char * itoaHex(uint64_t num, char * str) {
    int i = 0;
    while (num != 0) {
        int r = num % 16;
        str[i++] = (r < 10) ? (r + '0') : (r - 10 + 'A');
        num /= 16;
    }
    str[i] = 0;
    reverse(str);
    return str;
}

int printf(const char *fmt, ...) {
    int count = 0;
    va_list args;
    va_start(args, fmt);

    char buffer[PRINTF_BUFFER_SIZE];
    char *str;
    int num;
    uint64_t hex;

    for (int i=0; fmt[i] != 0; i++) {
        if (fmt[i] == '%') {
            switch (fmt[++i]) {
                // String
                case 's':
                    str = va_arg(args, char *);
                    for (int j=0; str[j] != '\0'; j++) {
                        putchar(str[j]);
                        count++;
                    }
                    break;
                // Integer
                case 'd':
                    num = va_arg(args, int);
                    itoa(num, buffer);
                    for (int j=0; buffer[j] != '\0'; j++) {
                        putchar(buffer[j]);
                        count++;
                    }
                    break;
                // Character
                case 'c':
                    putchar(va_arg(args, int));
                    count++;
                    break;
                case 'x':
                    hex = va_arg(args, uint64_t);
                    itoaHex(hex, buffer);
                    putchar('0');
                    putchar('x');
                    count += 2;
                    int digits = 16 - strlen(buffer);
                    for (int j = 0; j < digits; j++) {
                        putchar('0');
                        count++;
                    }
                    for (int j = 0; buffer[j] != '\0'; j++) {
                        putchar(buffer[j]);
                        count++;
                    }
                    break;
                // No special format found
                default:
                    putchar(fmt[i]);
                    count++;
                    break;
            }
        }
        else {
            // No special format
            putchar(fmt[i]);
            count++;
        }
    }
    va_end(args);
    return count;
}

int scanf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char input[SCANF_BUFFER_SIZE];
    fgets(input, SCANF_BUFFER_SIZE);
    int index = 0;

    // Aux buffer
    char buffer[SCANF_BUFFER_SIZE];
    void * ptr;
    int offset;

    int count = 0;
    for (int i=0; fmt[i] != 0; i++) {
        if (fmt[i] == '%') {
            switch (fmt[++i]) {
                // String
                case 's':
                    ptr = (void *) va_arg(args, char *);
                    offset = readToBlank(input, index);
                    strncpy(ptr, input + index, offset);
                    index += (offset + 1);
                    count++;
                    break;
                // Integer
                case 'd':
                    ptr = (void *) va_arg(args, int *);
                    offset = readToBlank(input, index);
                    strncpy(buffer, input + index, offset);
                    *(int *)ptr = atoi(buffer);
                    index += (offset + 1);
                    count++;
                    break;
                // Character
                case 'c':
                    if(input[index] != 0) {
                        *(char *)va_arg(args, char *) = input[index];
                        index++;
                    }
                    count++;
                    break;
                // No special format found
                default:
                    index++;
                    break;
            }
        }
    }
    va_end(args);
    return count;
}

void puts(const char *str) {
    for(int i=0; str[i] != 0; i++) {
        putchar(str[i]);
    }
    putchar('\n');
}

unsigned int getCursorX() {
    return _sys_getCoords() & 0xFFFFFFFF;
}

unsigned int getCursorY() {
    return _sys_getCoords() >> 32;
}

void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height) {
    _sys_drawRectangle(hexColor, x, y, width, height);
}

void clearScreen() {
    _sys_clearScreen();
}

unsigned int getScreenWidth() {
    return _sys_getScreenInfo() >> 32;
}

unsigned int getScreenHeight() {
    return _sys_getScreenInfo() & 0xFFFFFFFF;
}

unsigned int getFontWidth() {
    return _sys_getFontInfo() >> 32;
}

unsigned int getFontHeight() {
    return _sys_getFontInfo() & 0xFFFFFFFF;
}

int setFontScale(int scale) {
    return _sys_setFontScale(scale);
}

char toLower(char c) {
    if(c >= 'A' && c <= 'Z') {
        return c - 'A' + 'a';
    }
    return c;
}