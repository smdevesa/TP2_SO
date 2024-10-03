//
// Created by Santiago Devesa on 02/10/2024.
//

#ifndef TP2_SO_PROCESS_H
#define TP2_SO_PROCESS_H

#include <stdint.h>

typedef enum status {
    READY,
    BLOCKED,
    RUNNING,
    ZOMBIE,
    DEAD
} processStatus_t;

typedef struct Process {
    uint16_t pid;
    uint16_t parentPid;
    uint16_t waitingForPid;
    void *stackBase;
    void *stackPos;
    char **argv;
    char *name;
    processStatus_t status;
    uint8_t unkillable;
    int32_t retValue;
} process_t;

process_t *createProcessStructure(uint16_t pid, uint16_t parentPid, uint16_t waitingForPid, char **argv, char *name, uint8_t unkillable);
void freeProcessStructure(process_t *p);

#endif //TP2_SO_PROCESS_H
