#include <scheduler.h>
#include <process.h>
#include <memoryPositions.h>
#include <stddef.h>
#include <syscall_lib.h>
#include <lib.h>

extern void _hlt();
extern void _forceNextProcess();

#define NO_PID -1

typedef struct schedulerCDT {
    process_t *processes[MAX_PROCESSES];
    int16_t current;
    uint8_t processCount;
} schedulerCDT;

// avoid malloc
static processInfo_t psInfo[MAX_PROCESSES + 1];

static schedulerADT scheduler = NULL;

static int initProcessMain(int argc, char **argv);
static process_t * getNextProcess();
static void adoptChildren(int16_t pid);

static int initProcessMain(int argc, char **argv) {
    sys_write(1, "Initializing scheduler\n", 23, 0x00FFFFFF);
    char ** args = {NULL};
    addProcess((mainFunction)SHELL_ADDRESS, args, "shell",
               4, 0);

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
        yield();
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

    char *argv[] = {NULL};
    addProcess((mainFunction)&initProcessMain, argv, "init", MIN_PRIORITY, 1);
    return scheduler;
}

schedulerADT getScheduler() {
    return scheduler;
}


static process_t * getNextProcess() {
    if (scheduler->processCount == 0) return NULL;

    process_t *currentProcess = scheduler->processes[scheduler->current];
    if (currentProcess != NULL && currentProcess->remainingQuantum > 0) {
        currentProcess->remainingQuantum--;
        return currentProcess;
    }

    uint16_t start = (scheduler->current == NO_PID) ? 0 : (scheduler->current + 1) % MAX_PROCESSES;
    uint16_t current = start;

    do {
        if (scheduler->processes[current] != NULL &&
        scheduler->processes[current]->status == READY) {
            scheduler->current = current;
            scheduler->processes[current]->remainingQuantum = scheduler->processes[current]->priority - 1;
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
    scheduler->current = nextProcess->pid;
    uint64_t nextRSP = (uint64_t)nextProcess->stackPos;
    nextProcess->status = RUNNING;

    return (void *)nextRSP;
}



int64_t addProcess(mainFunction main, char **argv, char *name, uint8_t priority, uint8_t unkillable) {
    if(scheduler == NULL) return NO_PID;
    if(scheduler->processCount >= MAX_PROCESSES) return NO_PID;
    if(priority < MIN_PRIORITY || priority > MAX_PRIORITY) return NO_PID;

    uint16_t newPid;
    for(newPid = 0; newPid < MAX_PROCESSES; newPid++) {
        if(scheduler->processes[newPid] == NULL) break;
    }
    uint16_t parentPid = (scheduler->current != NO_PID) ? scheduler->current : NO_PID;

    process_t *newProcess = createProcessStructure(newPid, parentPid, NO_PID, main, argv, name, priority, unkillable);
    if (newProcess == NULL) return NO_PID;

    scheduler->processes[newPid] = newProcess;
    scheduler->processCount++;

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

int32_t killProcess(uint16_t pid, int32_t retValue) {
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if(scheduler->processes[pid]->unkillable) return -1;

    adoptChildren(pid);
    uint8_t contextSwitch = scheduler->processes[pid]->status == RUNNING;
    freeProcessStructure(scheduler->processes[pid]);
    scheduler->processes[pid] = NULL;
    scheduler->processCount--;
    if(contextSwitch){
        yield();
    }

    return 0;
}

int blockProcess(uint16_t pid){
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if (scheduler->processes[pid]->status == TERMINATED) return -1;

    uint8_t contextSwitch = scheduler->processes[pid]->status == RUNNING;
    scheduler->processes[pid]->status = BLOCKED;

    if(contextSwitch){
        yield();
    }
    return 0;
}

int unblockProcess(uint16_t pid){
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if (scheduler->processes[pid]->status != BLOCKED) return -1;
    scheduler->processes[pid]->status = READY;
    return 0;
}

int changePriority(uint16_t pid, uint8_t newPriority){
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if(newPriority < MIN_PRIORITY || newPriority > MAX_PRIORITY) return -1;

    scheduler->processes[pid]->priority = newPriority;
    scheduler->processes[pid]->remainingQuantum = newPriority;
    return 0;
}


int32_t killCurrentProcess(int32_t retValue) {
    return killProcess(scheduler->current, retValue);
}

static void adoptChildren(int16_t pid) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(scheduler->processes[i] != NULL && scheduler->processes[i]->parentPid == pid) {
            scheduler->processes[i]->parentPid = 0; // 0 is the init process
        }
    }
}

void yield() {
    _forceNextProcess();
}

uint16_t getPid() {
    return scheduler->current;
}

processInfo_t * ps() {
    int count = 0;
    for(int i=0; i<MAX_PROCESSES; i++) {
        if(scheduler->processes[i] != NULL) {
            processInfo_t info;
            info.pid = scheduler->processes[i]->pid;
            strncpy(info.name, scheduler->processes[i]->name, MAX_NAME_LENGTH);
            info.priority = scheduler->processes[i]->priority;
            info.unkillable = scheduler->processes[i]->unkillable;
            info.stackBase = scheduler->processes[i]->stackBase;
            info.parent = scheduler->processes[i]->parentPid;
            info.status = scheduler->processes[i]->status;
            psInfo[count++] = info;
        }
    }
    processInfo_t empty;
    empty.pid = -1;
    psInfo[count] = empty;
    return psInfo;
}
