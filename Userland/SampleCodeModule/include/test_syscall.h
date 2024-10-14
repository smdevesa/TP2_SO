#ifndef _TEST_SYSCALL_H
#define _TEST_SYSCALL_H

#include <stdint.h>
typedef int (*mainFunction)(int argc, char **argv);

int64_t my_getpid();
int64_t my_create_process(mainFunction main, char **argv, char *name, uint8_t priority, uint8_t unkillable);
int64_t my_nice(uint64_t pid, uint64_t newPrio);
int64_t my_kill(uint64_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
int64_t my_sem_open(char *sem_id, uint64_t initialValue);
int64_t my_sem_wait(char *sem_id);
int64_t my_sem_post(char *sem_id);
int64_t my_sem_close(char *sem_id);
int64_t my_yield();
int64_t my_wait(int64_t pid);

#endif
