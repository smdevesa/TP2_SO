//
// Created by Santiago Devesa on 23/10/2024.
//

#ifndef TP2_SO_SEMAPHORE_H
#define TP2_SO_SEMAPHORE_H

#define MAX_SEMAPHORES 64
#define MAX_SEM_NAME_LENGTH 64

#include <stdint.h>

void initSemManager();
int64_t semOpen(char * name, int initialValue);
int64_t semClose(char * name);
int64_t semWait(char * name);

#endif //TP2_SO_SEMAPHORE_H
