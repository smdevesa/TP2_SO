#include <stdint.h>
#include <syscall_lib.h>

enum syscalls {READ = 0, WRITE, RECTANGLE, CLEAR, COORDS, SCREENINFO, FONTINFO, GETTIME, SETSCALE,
        GETREGS, SLEEP, PLAYSOUND, YIELD, GETPID, CREATE_PROCESS, KILL_PROCESS, BLOCK, UNBLOCK, CHANGE_PRIORITY,
        EXIT, MALLOC, FREE, PS, SEM_OPEN, SEM_CLOSE, SEM_WAIT, SEM_POST, WAITPID, CREATE_PIPE, DESTROY_PIPE,
        GET_MEM_INFO, SYSCALLS_AMOUNT};

typedef int64_t (*syscall)(int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8);

static syscall syscall_functions[] = {
        (syscall) sys_read,
        (syscall) sys_write,
        (syscall) sys_draw_rectangle,
        (syscall) sys_clear_screen,
        (syscall) sys_get_coords,
        (syscall) sys_get_screen_info,
        (syscall) sys_get_font_info,
        (syscall) sys_get_time,
        (syscall) sys_set_font_scale,
        (syscall) sys_get_registers,
        (syscall) sys_sleep,
        (syscall) sys_play_sound,
        (syscall) sys_yield,
        (syscall) sys_getpid,
        (syscall) sys_create_process,
        (syscall) sys_kill_process,
        (syscall) sys_block_process,
        (syscall) sys_unblock_process,
        (syscall) sys_change_priority,
        (syscall) sys_exit,
        (syscall) sys_malloc,
        (syscall) sys_free,
        (syscall) sys_ps,
        (syscall) sys_sem_open,
        (syscall) sys_sem_close,
        (syscall) sys_sem_wait,
        (syscall) sys_sem_post,
        (syscall) sys_waitpid,
        (syscall) sys_create_pipe,
        (syscall) sys_destroy_pipe,
        (syscall) sys_get_mem_info
};


int64_t syscall_dispatcher(int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t rax) {
    if(rax < 0 || rax >= SYSCALLS_AMOUNT) {
        return -1;
    }

    return syscall_functions[rax](rdi, rsi, rdx, rcx, r8);
}