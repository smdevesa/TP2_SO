//
// Created by Santiago Devesa on 02/10/2024.
//

#ifndef TP2_SO_SCHEDULER_H
#define TP2_SO_SCHEDULER_H

#include <stdint.h>
#include <process.h>

#define MAX_PROCESSES 32
#define MAX_PRIORITY 4

typedef struct schedulerCDT * schedulerADT;

schedulerADT createScheduler();
schedulerADT getScheduler();
int64_t addProcess(mainFunction main, char **argv, char *name,uint8_t priority, uint8_t unkillable);
void freeScheduler();

#endif //TP2_SO_SCHEDULER_H
