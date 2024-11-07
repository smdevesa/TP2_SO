#include <stdint.h>
#include "../include/syscalls.h"

int64_t my_getpid() {
    return _sys_getpid();
}

int64_t my_create_process(mainFunction main, char **argv, char *name, uint8_t unkillable, int* fileDescriptors) {
    return _sys_createProcess(main, argv, name, unkillable, fileDescriptors);
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
    return _sys_semOpen(sem_id, initialValue);
}

int64_t my_sem_wait(char *sem_id) {
    return _sys_semWait(sem_id);
}

int64_t my_sem_post(char *sem_id) {
    return _sys_semPost(sem_id);
}

int64_t my_sem_close(char *sem_id) {
    return _sys_semClose(sem_id);
}

int64_t my_yield() {
    _sys_yield();
    return 0;
}

int64_t my_wait(int64_t pid) {
    return _sys_waitpid(pid);
}

void * my_malloc(uint64_t size) {
    return (void *)_sys_malloc(size);
}

void my_free(void * ptr) {
    _sys_free(ptr);
}