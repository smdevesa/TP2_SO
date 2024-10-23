//
// Created by Santiago Devesa on 23/10/2024.
//

#include <spinlock.h>
#include <semaphore.h>
#include <scheduler.h>
#include <memoryPositions.h>
#include <memory_manager.h>
#include <scheduler.h>

typedef struct {
    uint32_t v[MAX_PROCESSES];
    uint32_t readIndex;
    uint32_t writeIndex;
    uint32_t size;
} circular_buffer_t;

typedef struct {
    int value;
    lock_t lock;
    circular_buffer_t queue;
    uint32_t using[MAX_PROCESSES];
    uint32_t usingSize;
} semaphore_t;

typedef struct {
    semaphore_t *semaphores[MAX_SEMAPHORES];
    lock_t lock; // protect semaphores array
} semaphore_manager_t;

semaphore_manager_t * semaphoreManager = NULL;

static int64_t getFreeId();
static uint64_t popFromQueue(sem_t * sem);
static int addToQueue(sem_t * sem, uint32_t pid);
static int addToUsing(sem_t * sem, uint32_t pid);

static int64_t getFreeId() {
    for(int i=0; i<MAX_SEMAPHORES; i++) {
        if(semaphoreManager->semaphores[i] == NULL) {
            return i;
        }
    }
    return -1;
}

static uint64_t popFromQueue(sem_t * sem) {
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

static int addToQueue(sem_t * sem, uint32_t pid) {
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

static int addToUsing(sem_t * sem, uint32_t pid) {
    acquire(&sem->lock);
    if(sem->usingSize >= MAX_PROCESSES) {
        release(&sem->lock);
        return -1;
    }
    sem->using[sem->usingSize++] = pid;
    release(&sem->lock);
    return 0;
}

static int removeFromUsing(sem_t * sem, uint32_t pid) {
    acquire(&sem->lock);
    for(int i=0; i<sem->usingSize; i++) {
        if(sem->using[i] == pid) {
            sem->using[i] = sem->using[--sem->usingSize];
            release(&sem->lock);
            return 0;
        }
    }
    release(&sem->lock);
    return -1;
}

void initSemManager() {
    if(semaphoreManager != NULL) return;
    semaphoreManager = (semaphore_manager_t *) SEMAPHORE_ADDRESS;
    acquire(&semaphoreManager->lock);
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphoreManager->semaphores[i] = NULL;
    }
    release(&semaphoreManager->lock);
}

int64_t semInit(int initialValue) {
    acquire(&semaphoreManager->lock);
    int64_t id = getFreeId();
    if(id == -1) {
        release(&semaphoreManager->lock);
        return -1;
    }
    semaphoreManager->semaphores[id] = (semaphore_t *) my_malloc(sizeof(semaphore_t));
    semaphore->value = initialValue;
    semaphore->lock = 1;
    semaphore->queueSize = 0;
    semaphore->usingSize = 0;

    release(&semaphoreManager->lock);
    return id;
}

int64_t semDestroy(uint64_t semId) {
    acquire(&semaphoreManager->lock);
    if(semId >= MAX_SEMAPHORES) {
        release(&semaphoreManager->lock);
        return -1;
    }
    semaphore_t sem = semaphoreManager->semaphores[semId];
    if(sem == NULL || sem->usingSize > 0) {
        release(&semaphoreManager->lock);
        return -1;
    }
    my_free(sem);
    semaphoreManager->semaphores[semId] = NULL;
    release(&semaphoreManager->lock);
    return 0;
}

int64_t semOpen(uint64_t semId) {
    acquire(&semaphoreManager->lock);
    if(semId >= MAX_SEMAPHORES) {
        release(&semaphoreManager->lock);
        return -1;
    }
    semaphore_t sem = semaphoreManager->semaphores[semId];
    if(sem == NULL) {
        release(&semaphoreManager->lock);
        return -1;
    }
    if(addToUsing(sem, getPid()) == -1) {
        release(&semaphoreManager->lock);
        return -1;
    }
    release(&semaphoreManager->lock);
    return 0;
}

int64_t semClose(uint64_t semId) {
    acquire(&semaphoreManager->lock);
    if(semId >= MAX_SEMAPHORES) {
        release(&semaphoreManager->lock);
        return -1;
    }
    semaphore_t sem = semaphoreManager->semaphores[semId];
    if(sem == NULL) {
        release(&semaphoreManager->lock);
        return -1;
    }
    if(removeFromUsing(sem, getPid()) == -1) {
        release(&semaphoreManager->lock);
        return -1;
    }

    release(&semaphoreManager->lock);
    return 0;
}

int64_t semPost(uint64_t semId) {
    acquire(&semaphoreManager->lock);
    if(semId >= MAX_SEMAPHORES) {
        release(&semaphoreManager->lock);
        return -1;
    }
    semaphore_t sem = semaphoreManager->semaphores[semId];
    if(sem == NULL) {
        release(&semaphoreManager->lock);
        return -1;
    }
    sem->value++;
    if(sem->value <= 0) {
        uint32_t pid = popFromQueue(sem);
        if(pid != -1) {
            addToUsing(sem, pid);
            unblockProcess(pid);
        }
    }
    release(&semaphoreManager->lock);
    return 0;
}

int64_t semWait(uint64_t semId) {
    acquire(&semaphoreManager->lock);
    if(semId >= MAX_SEMAPHORES) {
        release(&semaphoreManager->lock);
        return -1;
    }
    semaphore_t sem = semaphoreManager->semaphores[semId];
    if(sem == NULL) {
        release(&semaphoreManager->lock);
        return -1;
    }
    sem->value--;
    if(sem->value < 0) {
        if(addToQueue(sem, getPid()) == -1) {
            release(&semaphoreManager->lock);
            return -1;
        }
        blockProcess(getPid());
    }
    release(&semaphoreManager->lock);
    return 0;
}
