#include <stdint.h>
#include "../include/syscalls.h"

int64_t my_getpid() {
    return _sys_getpid();
}

int64_t my_create_process(mainFunction main, char **argv, char *name, uint8_t priority, uint8_t unkillable) {
    return _sys_createProcess(main, argv, name, priority, unkillable);
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
    return _sys_changePriority(pid, newPrio);
}

int64_t my_kill(uint64_t pid) {
    return _sys_kill(pid);
}

int64_t my_block(uint64_t pid) {
    return _sys_block(pid);
}

int64_t my_unblock(uint64_t pid) {
    return _sys_unblock(pid);
}

int64_t my_sem_open(char *sem_id, uint64_t initialValue) {
    return 0;
}

int64_t my_sem_wait(char *sem_id) {
    return 0;
}

int64_t my_sem_post(char *sem_id) {
    return 0;
}

int64_t my_sem_close(char *sem_id) {
    return 0;
}

int64_t my_yield() {
    _sys_yield();
    return 0;
}

int64_t my_wait(int64_t pid) {
    return 0;
}
