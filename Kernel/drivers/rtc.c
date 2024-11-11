// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <rtc.h>

// We return 64 bits because of the syscall handler but we only need 8 bits
uint64_t getTime(uint64_t arg) {
    return _readTime(arg);
}
