//
// Created by Santiago Devesa on 02/10/2024.
//

#include <process.h>
#include <memory_manager.h>
#include <stddef.h>

#define STACK_SIZE 4096

process_t * createProcessStructure(uint16_t pid, uint16_t parentPid, uint16_t waitingForPid, mainFunction main, char **argv, char *name, uint8_t priority, uint8_t unkillable) {
    process_t * p = my_malloc(sizeof(process_t));
    if(p == NULL) return NULL;

    p->pid = pid;
    p->parentPid = parentPid;
    p->waitingForPid = waitingForPid;
    p->stackBase = my_malloc(STACK_SIZE);
    p->priority = priority;
    if(p->stackBase == NULL) {
        my_free(p);
        return NULL;
    }
    p->stackPos = p->stackBase + STACK_SIZE;
    p->main = main;
    p->argv = argv;
    p->name = name; //HICIERON UN ALLOC Y STRCPY
    p->unkillable = unkillable;
    p->status = READY;

    return p;
}

void freeProcessStructure(process_t *p) {
    my_free(p->stackBase);
    my_free(p);
}

