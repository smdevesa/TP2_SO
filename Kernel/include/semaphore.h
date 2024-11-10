//
// Created by Santiago Devesa on 23/10/2024.
//

#ifndef TP2_SO_SEMAPHORE_H
#define TP2_SO_SEMAPHORE_H

#define MAX_SEMAPHORES 256
#define MAX_SEM_NAME_LENGTH 64

#include <stdint.h>

void initSemManager();
int64_t semOpen(char * name, int initialValue);
int64_t semClose(char * name);
int64_t semWait(char * name);
int64_t semPost(char * name);
int remove_process_from_all_semaphore_queues(uint32_t pid);

#endif //TP2_SO_SEMAPHORE_H
