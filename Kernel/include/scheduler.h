//
// Created by Santiago Devesa on 02/10/2024.
//

#ifndef TP2_SO_SCHEDULER_H
#define TP2_SO_SCHEDULER_H

#include <stdint.h>
#include <process.h>

#define MAX_PROCESSES 32
#define MAX_PRIORITY 4
#define MIN_PRIORITY 1

typedef struct processInfo {
    int16_t pid;
    int16_t parent;
    char name[MAX_NAME_LENGTH];
    uint8_t priority;
    uint8_t unkillable;
    void *stackBase;
    processStatus_t status;
} processInfo_t;

typedef struct schedulerCDT * schedulerADT;

schedulerADT createScheduler();
schedulerADT getScheduler();
void * schedule(void *prevRSP);
int64_t addProcess(mainFunction main, char **argv, char *name,uint8_t priority, uint8_t unkillable);
void freeScheduler();
void yield();
uint16_t getPid();
int32_t killProcess(uint16_t pid, int32_t retValue);
int32_t killCurrentProcess(int32_t retValue);
int blockProcess(uint16_t pid); 
int unblockProcess(uint16_t pid);
int changePriority(uint16_t pid, uint8_t newPriority);
processInfo_t * ps();

#endif //TP2_SO_SCHEDULER_H
