#include <stdint.h>
#include <syscall_lib.h>

enum syscalls {READ = 0, WRITE, RECTANGLE, CLEAR, COORDS, SCREENINFO, FONTINFO, GETTIME, SETSCALE, GETREGS, SLEEP, PLAYSOUND};

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t rax) {
    // rax contains the syscall id
    switch (rax) {
        case READ: return sys_read(rdi, (char *) rsi, rdx);
        case WRITE: return sys_write(rdi, (char *) rsi, rdx, rcx);
        case RECTANGLE: return sys_drawRectangle(rdi, rsi, rdx, rcx, r8);
        case CLEAR: return sys_clearScreen();
        case COORDS: return sys_getCoords();
        case SCREENINFO: return sys_getScreenInfo();
        case FONTINFO: return sys_getFontInfo();
        case GETTIME: return sys_getTime(rdi);
        case SETSCALE: return sys_setFontScale(rdi);
        case GETREGS: return sys_getRegisters((uint64_t *) rdi);
        case SLEEP: return sys_sleep(rdi);
        case PLAYSOUND: return sys_playSound(rdi, rsi);
        default: return 0;
    }
}