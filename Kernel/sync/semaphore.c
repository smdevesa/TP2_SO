// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <spinlock.h>
#include <semaphore.h>
#include <scheduler.h>
#include <memoryPositions.h>
#include <memory_manager.h>
#include <scheduler.h>
#include <lib.h>
#include <syscall_lib.h>

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
    circular_buffer_t queue;
    lock_t lock;
} semaphore_t;

typedef struct {
    semaphore_t *semaphores[MAX_SEMAPHORES];
    int semaphoresCount;
} semaphore_manager_t;

semaphore_manager_t * semaphoreManager = NULL;

static int64_t getFreeId();
static uint64_t popFromQueue(semaphore_t * sem);
static int addToQueue(semaphore_t * sem, uint32_t pid);
static semaphore_t * getSemByName(char * name);
static int getIdxByName(char * name);
static int remove_process_from_queue(semaphore_t *sem, uint32_t pid);

        static int64_t getFreeId() {
    for(int i=0; i<MAX_SEMAPHORES; i++) {
        if(semaphoreManager->semaphores[i] == NULL) {
            return i;
        }
    }
    return -1;
}

static uint64_t popFromQueue(semaphore_t * sem) {
    if(sem->queue.size == 0) return -1;
    uint32_t pid = sem->queue.v[sem->queue.readIndex];
    sem->queue.readIndex = (sem->queue.readIndex + 1) % MAX_PROCESSES;
    sem->queue.size--;
    return pid;
}

static int addToQueue(semaphore_t * sem, uint32_t pid) {
    if(sem->queue.size >= MAX_PROCESSES) return -1;
    sem->queue.v[sem->queue.writeIndex] = pid;
    sem->queue.writeIndex = (sem->queue.writeIndex + 1) % MAX_PROCESSES;
    sem->queue.size++;
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
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphoreManager->semaphores[i] = NULL;
    }
    semaphoreManager->semaphoresCount = 0;
}

int64_t semOpen(char * name, int initialValue) {
    if(semaphoreManager == NULL) return -1;
    if(semaphoreManager->semaphoresCount >= MAX_SEMAPHORES) return -1;

    semaphore_t * sem = getSemByName(name);
    if(sem != NULL) {
        acquire(&sem->lock);
        sem->using++;
        release(&sem->lock);
        return 0;
    }
    int64_t id = getFreeId();
    if(id == -1) {
        return -1;
    }
    sem = (semaphore_t *) my_malloc(sizeof(semaphore_t));
    if(sem == NULL) {
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
    return 0;
}

int64_t semClose(char * name) {
    if(semaphoreManager == NULL) return -1;
    int idx = getIdxByName(name);
    if(idx == -1) {
        return -1;
    }

    semaphore_t * sem = semaphoreManager->semaphores[idx];
    if(sem->using > 1) {
        acquire(&sem->lock);
        sem->using--;
        release(&sem->lock);
        return 0;
    }

    my_free(sem);
    semaphoreManager->semaphores[idx] = NULL;
    semaphoreManager->semaphoresCount--;
    return 0;
}

int64_t semWait(char * name) {
    if (semaphoreManager == NULL) return -1;

    semaphore_t * sem = getSemByName(name);

    if (sem == NULL) return -1;

    acquire(&sem->lock);
    if (sem->value > 0) {
        sem->value--;
        release(&sem->lock);
        return 0;
    }
    uint16_t pid = getPid();
    if (addToQueue(sem, pid) == -1) {
        release(&sem->lock);
        return -1;
    }
    release(&sem->lock);
    blockProcess(pid);
    return 0;
}


int64_t semPost(char * name) {
    if (semaphoreManager == NULL) return -1;

    semaphore_t * sem = getSemByName(name);
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

static int remove_process_from_queue(semaphore_t *sem, uint32_t pid) {
    if (sem->queue.size == 0) return -1;

    uint32_t i = sem->queue.readIndex;
    uint32_t j = 0;
    int found = -1;

    while (j < sem->queue.size) {
        if (sem->queue.v[i] == pid) {
            found = i;
            break;
        }
        i = (i + 1) % MAX_PROCESSES;
        j++;
    }

    if (found == -1) return -1;
    for (uint32_t k = found; k != sem->queue.writeIndex; k = (k + 1) % MAX_PROCESSES) {
        uint32_t next = (k + 1) % MAX_PROCESSES;
        sem->queue.v[k] = sem->queue.v[next];
    }

    sem->queue.writeIndex = (sem->queue.writeIndex + MAX_PROCESSES - 1) % MAX_PROCESSES;
    sem->queue.size--;

    return 0;
}

int remove_process_from_all_semaphore_queues(uint32_t pid) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphore_t *sem = semaphoreManager->semaphores[i];
        if (sem == NULL) continue;
        acquire(&sem->lock);
        remove_process_from_queue(sem, pid);
        release(&sem->lock);
    }
    return 0;
}
