#include <stdint.h>
#include <syscall_lib.h>

enum syscalls {READ = 0, WRITE, RECTANGLE, CLEAR, COORDS, SCREENINFO, FONTINFO, GETTIME, SETSCALE,
        GETREGS, SLEEP, PLAYSOUND, YIELD, GETPID, CREATE_PROCESS, KILL_PROCESS, BLOCK, UNBLOCK, CHANGE_PRIORITY,
        EXIT, MALLOC, FREE, PS, SEM_OPEN, SEM_CLOSE, SEM_WAIT, SEM_POST, WAITPID};

uint64_t syscallDispatcher(int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t rax) {
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
        case YIELD: return sys_yield();
        case GETPID: return sys_getPid();
        case CREATE_PROCESS: return sys_createProcess(rdi, (char **) rsi, (char *) rdx, (uint8_t) rcx, (uint8_t) r8);
        case KILL_PROCESS: return sys_killProcess(rdi);
        case BLOCK: return sys_blockProcess(rdi);
        case UNBLOCK: return sys_unblockProcess(rdi);
        case CHANGE_PRIORITY: return sys_changePriority(rdi, rsi);
        case EXIT: return sys_exit(rdi);
        case MALLOC: return sys_malloc(rdi);
        case FREE: return sys_free(rdi);
        case PS: return sys_ps();
        case SEM_OPEN: return sys_semOpen((char *) rdi, rsi);
        case SEM_CLOSE: return sys_semClose((char *) rdi);
        case SEM_WAIT: return sys_semWait((char *) rdi);
        case SEM_POST: return sys_semPost((char *) rdi);
        case WAITPID: return sys_waitpid(rdi);
        default: return 0;
    }
}