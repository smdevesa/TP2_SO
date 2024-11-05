#ifndef TP2_SO_PROCESS_H
#define TP2_SO_PROCESS_H

#include <stdint.h>

#define MAX_NAME_LENGTH 256

typedef int (*mainFunction)(int argc, char **argv);

typedef enum status {
    READY = 0,
    BLOCKED,
    RUNNING,
    TERMINATED
} processStatus_t;

typedef struct process {
    uint16_t pid;
    int16_t parentPid;
    int16_t waitingForPid;
    void *stackBase;
    void *stackPos;
    mainFunction main;
    char **argv;
    char name[MAX_NAME_LENGTH];
    processStatus_t status;
    uint8_t priority;
    uint8_t remainingQuantum;
    uint8_t unkillable;
    uint16_t writeFd;
    uint16_t readFd;
    int32_t retValue;
} process_t;

process_t * createProcessStructure(uint16_t pid, uint16_t parentPid, uint16_t waitingForPid, mainFunction main,
                                   char **argv, char *name, uint8_t priority, uint8_t unkillable,
                                   uint16_t writeFd, uint16_t readFd);

void freeProcessStructure(process_t *p);

#endif //TP2_SO_PROCESS_H
