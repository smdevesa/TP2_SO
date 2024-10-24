//
// Created by Santiago Devesa on 23/10/2024.
//

#include <spinlock.h>
#include <semaphore.h>
#include <scheduler.h>
#include <memoryPositions.h>
#include <memory_manager.h>
#include <scheduler.h>
#include <lib.h>

typedef struct {
    uint32_t v[MAX_PROCESSES];
    uint32_t readIndex;
    uint32_t writeIndex;
    uint32_t size;
} circular_buffer_t;

typedef struct {
    int value;
    int using;
    char name[MAX_SEM_NAME_LENGTH];
    lock_t lock;
    circular_buffer_t queue;
} semaphore_t;

typedef struct {
    semaphore_t *semaphores[MAX_SEMAPHORES];
    int semaphoresCount;
    lock_t lock; // protect semaphores array
} semaphore_manager_t;

semaphore_manager_t * semaphoreManager = NULL;

static int64_t getFreeId();
static uint64_t popFromQueue(semaphore_t * sem);
static int addToQueue(semaphore_t * sem, uint32_t pid);
static semaphore_t * getSemByName(char * name);
static int getIdxByName(char * name);

static int64_t getFreeId() {
    for(int i=0; i<MAX_SEMAPHORES; i++) {
        if(semaphoreManager->semaphores[i] == NULL) {
            return i;
        }
    }
    return -1;
}

static uint64_t popFromQueue(semaphore_t * sem) {
    acquire(&sem->lock);
    if(sem->queue.size == 0) {
        release(&sem->lock);
        return -1;
    }
    uint32_t pid = sem->queue.v[sem->queue.readIndex];
    sem->queue.readIndex = (sem->queue.readIndex + 1) % MAX_PROCESSES;
    sem->queue.size--;
    release(&sem->lock);
    return pid;
}

static int addToQueue(semaphore_t * sem, uint32_t pid) {
    acquire(&sem->lock);
    if(sem->queue.size >= MAX_PROCESSES) {
        release(&sem->lock);
        return -1;
    }
    sem->queue.v[sem->queue.writeIndex] = pid;
    sem->queue.writeIndex = (sem->queue.writeIndex + 1) % MAX_PROCESSES;
    sem->queue.size++;
    release(&sem->lock);
    return 0;
}

static int getIdxByName(char * name) {
    for(int i=0; i<MAX_SEMAPHORES; i++) {
        if(semaphoreManager->semaphores[i] != NULL && strcmp(semaphoreManager->semaphores[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static semaphore_t * getSemByName(char * name) {
    int idx = getIdxByName(name);
    if(idx == -1) return NULL;
    return semaphoreManager->semaphores[idx];
}

void initSemManager() {
    if(semaphoreManager != NULL) return;
    semaphoreManager = (semaphore_manager_t *) SEMAPHORE_ADDRESS;
    acquire(&semaphoreManager->lock);
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphoreManager->semaphores[i] = NULL;
    }
    semaphoreManager->semaphoresCount = 0;
    release(&semaphoreManager->lock);
}

int64_t semOpen(char * name, int initialValue) {
    if(semaphoreManager == NULL) return -1;
    if(semaphoreManager->semaphoresCount >= MAX_SEMAPHORES) return -1;

    acquire(&semaphoreManager->lock);
    semaphore_t * sem = getSemByName(name);
    if(sem != NULL) {
        acquire(&sem->lock);
        sem->using++;
        release(&sem->lock);
        release(&semaphoreManager->lock);
        return 0;
    }
    int64_t id = getFreeId();
    if(id == -1) {
        release(&semaphoreManager->lock);
        return -1;
    }
    sem = (semaphore_t *) my_malloc(sizeof(semaphore_t));
    if(sem == NULL) {
        release(&semaphoreManager->lock);
        return -1;
    }
    sem->value = initialValue;
    strncpy(sem->name, name, MAX_SEM_NAME_LENGTH);
    sem->queue.readIndex = 0;
    sem->queue.writeIndex = 0;
    sem->queue.size = 0;
    sem->lock = 1;
    sem->using = 1;
    semaphoreManager->semaphores[id] = sem;
    semaphoreManager->semaphoresCount++;
    release(&semaphoreManager->lock);
    return 0;
}

int64_t semClose(char * name) {
    if(semaphoreManager == NULL) return -1;
    acquire(&semaphoreManager->lock);
    int idx = getIdxByName(name);
    if(idx == -1) {
        release(&semaphoreManager->lock);
        return -1;
    }

    semaphore_t * sem = semaphoreManager->semaphores[idx];
    if(sem->using > 1) {
        acquire(&sem->lock);
        sem->using--;
        release(&sem->lock);
        release(&semaphoreManager->lock);
        return 0;
    }

    my_free(sem);
    semaphoreManager->semaphores[idx] = NULL;
    semaphoreManager->semaphoresCount--;
    release(&semaphoreManager->lock);
    return 0;
}

int64_t semWait(char * name) {
    if (semaphoreManager == NULL) return -1;

    acquire(&semaphoreManager->lock);
    semaphore_t * sem = getSemByName(name);
    release(&semaphoreManager->lock);

    if (sem == NULL) return -1;

    acquire(&sem->lock);
    if (sem->value > 0) {
        sem->value--;
        release(&sem->lock);
        return 0;
    }
    uint16_t pid = getPid();
    if (addToQueue(sem, pid) != 0) {
        release(&sem->lock);
        return -1;
    }
    blockProcess(pid);
    release(&sem->lock);
    return 0;
}


int64_t semPost(char * name) {
    if (semaphoreManager == NULL) return -1;

    acquire(&semaphoreManager->lock);
    semaphore_t * sem = getSemByName(name);
    release(&semaphoreManager->lock);

    if (sem == NULL) return -1;

    acquire(&sem->lock);

    if (sem->queue.size > 0) {
        uint32_t pid = popFromQueue(sem);
        unblockProcess(pid);
    } else {
        sem->value++;
    }

    release(&sem->lock);
    return 0;
}
