#include <process.h>
#include <memory_manager.h>
#include <string.h>
#include <stddef.h>
#include <scheduler.h>

#define STACK_SIZE 4096
#define MAX_NAME_LENGTH 256

// create a fake iretq frame
extern void *_initialize_stack_frame(void (*wrapper)(mainFunction, char **), mainFunction main, void *stackEnd, void *argv);
static int argsLen(char **array);
static void copyName(char *dest, const char *src);
// calls the main function of the process and then kills it
void processCaller(mainFunction main, char **args);

process_t *createProcessStructure(uint16_t pid, uint16_t parentPid, uint16_t waitingForPid, mainFunction main, char **argv, char *name, uint8_t priority, uint8_t unkillable) {
    process_t *p = my_malloc(sizeof(process_t));
    if (p == NULL) return NULL;

    p->pid = pid;
    p->parentPid = parentPid;
    p->waitingForPid = waitingForPid;
    p->priority = priority;
    p->remainingQuantum = priority;
    p->unkillable = unkillable;
    p->status = READY;
    p->stackBase = my_malloc(STACK_SIZE);
    if (p->stackBase == NULL) {
        my_free(p);
        return NULL;
    }
    p->stackPos = p->stackBase + STACK_SIZE;
    p->argv = argv;
    copyName(p->name, name);
    p->name[MAX_NAME_LENGTH - 1] = 0;
    p->stackPos = _initialize_stack_frame(&processCaller, main, p->stackPos, (void *) p->argv);

    return p;
}

void freeProcessStructure(process_t *p) {
    if (p == NULL) return;
    my_free(p->stackBase);
    my_free(p);
}

static void copyName(char *dest, const char *src) {
    for (size_t i = 0; i < MAX_NAME_LENGTH; i++) {
        dest[i] = src[i];
        if (src[i] == '\0') break;
    }
}

void processCaller(mainFunction main, char **args) {
    int argc = argsLen(args);
    int retValue = main(argc, args);
    killCurrentProcess(retValue);
}

static int argsLen(char **array) {
    int i = 0;
    while (array[i] != NULL) i++;
    return i;
}
