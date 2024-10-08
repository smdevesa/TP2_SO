//
// Created by Santiago Devesa on 02/10/2024.
//

#include <scheduler.h>
#include <process.h>
#include <memoryPositions.h>
#include <stddef.h>

#define NO_PID -1

typedef struct schedulerCDT {
    process_t *processes[MAX_PROCESSES];
    process_t *processesPriority[MAX_PROCESSES * MAX_PRIORITY];
    int16_t currentPid;
    uint8_t nextUnusedPid;
    uint8_t processCount;
    uint8_t totalPriorityCount;
} schedulerCDT;

static schedulerADT scheduler = NULL;

static int16_t getNextProcessPID();

schedulerADT createScheduler() {
    if(scheduler != NULL) return scheduler;
    scheduler = (schedulerADT) SCHEDULER_ADDRESS;
    for(int i = 0; i < MAX_PROCESSES; i++) {
        scheduler->processes[i] = NULL;
    }
    scheduler->nextUnusedPid = 0;
    scheduler->processCount = 0;
    scheduler->currentPid = NO_PID;
    return scheduler;
}

schedulerADT getScheduler() {
    return scheduler;
}

static int16_t getNextProcessPID() {
    if(scheduler->processCount == 0) return NO_PID;
    int16_t nextPid = scheduler->currentPid;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        nextPid = (nextPid + 1) % MAX_PROCESSES;
        if (scheduler->processes[nextPid] != NULL && scheduler->processes[nextPid]->status == READY) {
            return nextPid;
        }
    }
    return NO_PID;
}

void * schedule(void *prevRSP) {
    if(scheduler == NULL || scheduler->processCount == 0) return prevRSP;
    if(scheduler->processes[scheduler->currentPid] != NULL && scheduler->processes[scheduler->currentPid]->status == RUNNING) {
        scheduler->processes[scheduler->currentPid]->stackPos = prevRSP;
        scheduler->processes[scheduler->currentPid]->status = READY;
    }
    int16_t nextPid = getNextProcessPID();
    if(nextPid == NO_PID) return prevRSP;
    scheduler->currentPid = nextPid;
    scheduler->processes[nextPid]->status = RUNNING;
    return scheduler->processes[nextPid]->stackPos;
}

int64_t addProcess(mainFunction main, char **argv, char *name,uint8_t priority, uint8_t unkillable) {
    if(scheduler == NULL || scheduler->processCount >= MAX_PROCESSES) return NO_PID;
    process_t *p = createProcessStructure(scheduler->nextUnusedPid, scheduler->currentPid != NO_PID ? scheduler->currentPid : 0, NO_PID, main, argv, name, priority ,unkillable);

    if(p == NULL) return NO_PID;

    scheduler->processes[scheduler->nextUnusedPid] = p;

    for(int i = 0; i < priority; i++){
        scheduler -> processesPriority[scheduler->totalPriorityCount + i] = p;
    }
    scheduler->totalPriorityCount += priority;
    scheduler->processCount++;

    while(scheduler->processes[scheduler->nextUnusedPid] != NULL) {
        scheduler->nextUnusedPid = (scheduler->nextUnusedPid + 1) % MAX_PROCESSES;
    }
    return p->pid;
}

void freeScheduler() {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(scheduler->processes[i] != NULL) {
            freeProcessStructure(scheduler->processes[i]);
        }
    }
    scheduler = NULL;
}

int16_t killProcess(uint16_t pid) {
    if(scheduler == NULL || scheduler->processCount == 0) return -1;
    if(pid < 0 || pid >= MAX_PROCESSES || scheduler->processes[pid] == NULL) return -1;
    if(scheduler->processes[pid]->unkillable) return -1;

    freeProcessStructure(scheduler->processes[pid]);
    scheduler->processes[pid] = NULL;
    if(scheduler->processCount > 0) scheduler->processCount--;
    scheduler->nextUnusedPid = pid;
    return 0;
}
