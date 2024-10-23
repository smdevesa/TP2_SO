//
// Created by Santiago Devesa on 23/10/2024.
//

#include <spinlock.h>
#include <semaphore.h>
#include <scheduler.h>
#include <memoryPositions.h>
#include <memory_manager.h>

typedef struct {
    int value;
    lock_t lock;
    uint32_t queue[MAX_PROCESSES];
} semaphore_t;

typedef struct {
    semaphore_t *semaphores[MAX_SEMAPHORES];
    lock_t lock; // protect semaphores array
} semaphore_manager_t;

semaphore_manager_t * semaphoreManager = NULL;

int64_t getFreeId() {
    for(int i=0; i<MAX_SEMAPHORES; i++) {
        if(semaphoreManager->semaphores[i] == NULL) {
            return i;
        }
    }
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

    release(&semaphoreManager->lock);
    return id;
}

int64_t semDestroy(uint64_t semId) {
    acquire(&semaphoreManager->lock);
    if(semId >= MAX_SEMAPHORES || semaphoreManager->semaphores[semId] == NULL) {
        release(&semaphoreManager->lock);
        return -1;
    }
    my_free(semaphoreManager->semaphores[semId]);
    semaphoreManager->semaphores[semId] = NULL;
    release(&semaphoreManager->lock);
    return 0;
}