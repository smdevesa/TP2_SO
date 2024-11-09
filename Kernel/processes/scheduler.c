#include <scheduler.h>
#include <process.h>
#include <memoryPositions.h>
#include <stddef.h>
#include <lib.h>
#include <syscall_lib.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <pipes.h>
#include <time.h>

extern void _hlt();
extern void _forceNextProcess();

#define NO_PID -1
#define STDOUT 1
#define SHELL_PID 1

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
static void removeProcess(uint16_t pid);

static int initProcessMain(int argc, char **argv) {
    char ** args = {NULL};
    int fds[2] = {STDIN, STDOUT};
    int pid = addProcess((mainFunction)SHELL_ADDRESS, args, "shell",1, fds);
    changePriority(pid, MAX_PRIORITY);

    while(1) {
        for(int i=0; i<MAX_PROCESSES; i++) {
            if(scheduler->processes[i] != NULL) {
                if(scheduler->processes[i]->status == TERMINATED && scheduler->processes[i]->parentPid == 0) {
                    removeProcess(i);
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
    int fds[2] = {STDIN, STDOUT};
    addProcess((mainFunction)&initProcessMain, argv, "init", 1, fds);
    return scheduler;
}

schedulerADT getScheduler() {
    return scheduler;
}


static void removeProcess(uint16_t pid) {
    if (scheduler->processes[pid] != NULL) {
        freeProcessStructure(scheduler->processes[pid]);
        scheduler->processes[pid] = NULL;
        scheduler->processCount--;
    }
}

static process_t * getNextProcess() {
    if (scheduler->processCount == 0) return NULL;

    process_t *currentProcess = scheduler->processes[scheduler->current];
    if (currentProcess != NULL &&
    (currentProcess->status == READY || currentProcess->status == RUNNING) &&
    currentProcess->remainingQuantum > 0) {
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
        currentProcess->stackPos = prevRSP;
        if(currentProcess->status == RUNNING) {
            currentProcess->status = READY;
        }
    }

    process_t *nextProcess = getNextProcess();
    scheduler->current = nextProcess->pid;
    uint64_t nextRSP = (uint64_t)nextProcess->stackPos;
    nextProcess->status = RUNNING;

    char current[10];
    itoa(nextProcess->pid, current);
    for(int i=0; i<10; i++) {
        drawRectangle(0x00000000, 980+(i*8), 5, 8, 16);
    }
    for(int i=0; current[i] != 0; i++) {
        drawChar(current[i], 0x0000FF00, 0x00000000, 980+(i*8), 5);
    }

    return (void *)nextRSP;
}



int64_t addProcess(mainFunction main, char **argv, char *name, uint8_t unkillable, int* fileDescriptors) {
    if(scheduler == NULL) return NO_PID;
    if(scheduler->processCount >= MAX_PROCESSES) return NO_PID;
    if(fileDescriptors == NULL) return NO_PID;

    uint16_t newPid;
    for(newPid = 0; newPid < MAX_PROCESSES; newPid++) {
        if(scheduler->processes[newPid] == NULL) break;
    }
    uint16_t parentPid = (scheduler->current != NO_PID) ? scheduler->current : NO_PID;

    process_t *newProcess = createProcessStructure(newPid, parentPid, NO_PID, main,
                                                   argv, name, MIN_PRIORITY, unkillable,
                                                   fileDescriptors[1], fileDescriptors[0]);
    if (newProcess == NULL) return NO_PID;
    newProcess->waiting_for_stdin = 0;

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

int32_t killProcess(uint16_t pid) {
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if(scheduler->processes[pid]->unkillable) return -1;

    adoptChildren(pid);
    process_t *process = scheduler->processes[pid];
    process_t *parent = scheduler->processes[process->parentPid];
    if(parent != NULL && parent->status == BLOCKED && parent->waitingForPid == process->pid) {
        unblockProcess(parent->pid);
    }
    uint8_t contextSwitch = scheduler->processes[pid]->status == RUNNING;
    remove_sleeping_process(pid);
    if(process->writeFd != STDOUT) {
        send_pipe_eof(process->writeFd);
    }
    if(process->waiting_for_stdin) {
        release_stdin();
    }
    removeProcess(pid);

    if(contextSwitch){
        yield();
    }

    return 0;
}

int block_process_sleep(uint16_t pid, uint8_t sleep) {
    if(pid == 0) return -1;
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if(scheduler->processes[pid]->status == TERMINATED) return -1;

    if(!sleep) remove_sleeping_process(pid);

    uint8_t contextSwitch = pid == scheduler->current;
    scheduler->processes[pid]->status = BLOCKED;

    if(contextSwitch){
        yield();
    }
    return 0;
}

int blockProcess(uint16_t pid) {
    return block_process_sleep(pid, 0);
}

int unblockProcess(uint16_t pid){
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if(scheduler->processes[pid]->status != BLOCKED) return -1;

    remove_sleeping_process(pid);

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


int32_t killCurrentProcess() {
    return killProcess(scheduler->current);
}

static void adoptChildren(int16_t pid) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(scheduler->processes[i] != NULL && scheduler->processes[i]->parentPid == pid) {
            scheduler->processes[i]->parentPid = 0;
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

int64_t waitPid(uint32_t pid) {
    if(scheduler == NULL) return -1;
    if(pid >= MAX_PROCESSES) return -1;
    if(scheduler->processes[pid] == NULL) return -1;
    if(scheduler->processes[pid]->parentPid != scheduler->current) return -1;

    if(scheduler->processes[pid]->status != TERMINATED) {
        scheduler->processes[scheduler->current]->waitingForPid = pid;
        blockProcess(scheduler->current);
    }

    scheduler->processes[scheduler->current]->waitingForPid = NO_PID;
    int64_t retValue = scheduler->processes[pid]->retValue;
    removeProcess(pid);

    return retValue;
}

void my_exit(int64_t retValue) {
    if(scheduler == NULL) return;
    process_t *currentProcess = scheduler->processes[scheduler->current];

    if(currentProcess->writeFd != STDOUT) {
        send_pipe_eof(currentProcess->writeFd);
    }
    currentProcess->status = TERMINATED;
    currentProcess->retValue = retValue;
    remove_sleeping_process(currentProcess->pid);

    process_t * parent = scheduler->processes[currentProcess->parentPid];
    if(parent != NULL && parent->status == BLOCKED && parent->waitingForPid == currentProcess->pid) {
        unblockProcess(parent->pid);
    }
    yield();
}

void getCurrentFDs(int *fds) {
    if(scheduler == NULL) return;
    if(scheduler->current == NO_PID) return;
    process_t *currentProcess = scheduler->processes[scheduler->current];
    fds[0] = currentProcess->readFd;
    fds[1] = currentProcess->writeFd;
}

void kill_process_in_foreground() {
    if(scheduler == NULL) return;
    for(int i=0; i<MAX_PROCESSES; i++) {
        if(scheduler->processes[i] != NULL &&
        scheduler->processes[SHELL_PID]->waitingForPid == i) {
            killProcess(i);
            return;
        }
    }
}

void update_stdin_waiting(uint8_t value) {
    if(scheduler == NULL) return;
    if(scheduler->current == NO_PID) return;
    scheduler->processes[scheduler->current]->waiting_for_stdin = value;
}
