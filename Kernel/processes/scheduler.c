#include <scheduler.h>
#include <process.h>
#include <memoryPositions.h>
#include <stddef.h>
#include <syscall_lib.h>
#include <lib.h>

extern void _hlt();

#define NO_PID -1
#define PRIORITY_SIZE (MAX_PROCESSES * MAX_PRIORITY)

typedef struct schedulerCDT {
    process_t *processes[MAX_PROCESSES];
    int16_t current;
    uint8_t processCount;
} schedulerCDT;

static schedulerADT scheduler = NULL;

static int initProcessMain(int argc, char **argv);
static process_t * getNextProcess();

static int initProcessMain() {
    addProcess((mainFunction)SHELL_ADDRESS, (char **){NULL}, "Shell",
               MIN_PRIORITY, 1);

    while(1) {
        for(int i=0; i<MAX_PROCESSES; i++) {
            if(scheduler->processes[i] != NULL) {
                if(scheduler->processes[i]->status == TERMINATED) {
                    freeProcessStructure(scheduler->processes[i]);
                    scheduler->processes[i] = NULL;
                    scheduler->processCount--;
                }
            }
        }
    }

    return 0;
}

schedulerADT createScheduler() {
    if(scheduler != NULL) return scheduler;
    scheduler = (schedulerADT) SCHEDULER_ADDRESS;
    for(int i = 0; i < MAX_PROCESSES; i++) {
        scheduler->processes[i] = NULL;
    }
    scheduler->processCount = 0;
    scheduler->current = NO_PID;

    addProcess((mainFunction)&initProcessMain, (char **){NULL}, "Idle Process",
               MIN_PRIORITY, 1);
    return scheduler;
}

schedulerADT getScheduler() {
    return scheduler;
}


static process_t * getNextProcess() {
    if (scheduler->processCount == 0) return NULL;

    uint16_t start = (scheduler->current == NO_PID) ? 0 : (scheduler->current + 1) % MAX_PROCESSES;
    uint16_t current = start;

    do {
        if (scheduler->processes[current] != NULL &&
        scheduler->processes[current]->status == READY) {
            scheduler->current = current;
            return scheduler->processes[current];
        }
        current = (current + 1) % MAX_PROCESSES;
    } while (current != start);

    return scheduler->processes[0];
}

void * schedule(void *prevRSP) {
    if(scheduler == NULL) return prevRSP;
    if(scheduler->processCount == 0) return prevRSP;
    if (scheduler->current != NO_PID) {
        process_t *currentProcess = scheduler->processes[scheduler->current];
        currentProcess->status = READY;
        currentProcess->stackPos = prevRSP;
    }

    process_t *nextProcess = getNextProcess();
    if (nextProcess == NULL) {
        nextProcess = scheduler->processes[0];
    }

    scheduler->current = nextProcess->pid;
    uint64_t nextRSP = (uint64_t)nextProcess->stackPos;
    nextProcess->status = RUNNING;

    return (void *)nextRSP;
}



int64_t addProcess(mainFunction main, char **argv, char *name, uint8_t priority, uint8_t unkillable) {
    if(scheduler == NULL) return NO_PID;
    if(scheduler->processCount >= MAX_PROCESSES) return NO_PID;
    if(priority < MIN_PRIORITY || priority > MAX_PRIORITY) return NO_PID;

    uint16_t newPid = scheduler->processCount;
    uint16_t parentPid = (scheduler->current != NO_PID) ? scheduler->current : NO_PID;

    process_t *newProcess = createProcessStructure(newPid, parentPid, NO_PID, main, argv, name, priority, unkillable);
    if (newProcess == NULL) return NO_PID;

    scheduler->processes[newPid] = newProcess;
    scheduler->processCount++;

    sys_write(1, "Process ", 8, 0x00FFFFFF);
    char num = newPid + '0';
    sys_write(1, &num, 1, 0x00FFFFFF);
    sys_write(1, " created\n", 9, 0x00FFFFFF);
    return newPid;
}


void freeScheduler() {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(scheduler->processes[i] != NULL) {
            freeProcessStructure(scheduler->processes[i]);
        }
    }
    scheduler = NULL;
}

int16_t killProcess(uint16_t pid, int32_t retValue) {
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if(scheduler->processes[pid]->unkillable) return -1;

    scheduler->processes[pid]->status = TERMINATED;
    scheduler->processes[pid]->retValue = retValue;

    return 0;
}


int16_t killCurrentProcess(int32_t retValue) {
    return killProcess(scheduler->current, retValue);
}
