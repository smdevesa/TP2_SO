#include <process.h>
#include <memory_manager.h>
#include <string.h>
#include <stddef.h>
#include <scheduler.h>
#include <lib.h>

#define STACK_SIZE 4096
#define MAX_NAME_LENGTH 256

// create a fake iretq frame
extern void *_initialize_stack_frame(void (*wrapper)(mainFunction, char **), mainFunction main, void *stackEnd, void *argv);
static int argsLen(char **array);
static void acotatedCopy(char *dest, const char *src, size_t size);
// calls the main function of the process and then kills it
void processCaller(mainFunction main, char **args);
static char ** allocArgs(char **args);

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
    p->argv = allocArgs(argv);
    if(p->argv == NULL) {
        my_free(p->stackBase);
        my_free(p);
        return NULL;
    }
    acotatedCopy(p->name, name, MAX_NAME_LENGTH - 1);
    p->name[MAX_NAME_LENGTH - 1] = 0;
    p->stackPos = _initialize_stack_frame(&processCaller, main, p->stackPos, (void *) p->argv);

    return p;
}

void freeProcessStructure(process_t *p) {
    if (p == NULL) return;
    for (int i = 0; p->argv[i] != NULL; i++) {
        my_free(p->argv[i]);
    }
    my_free(p->stackBase);
    my_free(p);
}

static char ** allocArgs(char **args) {
    if (args == NULL || args[0] == NULL) {
        char **newArgs = my_malloc(sizeof(char *));
        if (newArgs == NULL) return NULL;
        newArgs[0] = NULL;
        return newArgs;
    }

    int count = argsLen(args);
    char ** newArgs = my_malloc((count+1) * sizeof(char *));
    for(int i=0; i<count; i++) {
        int len = strlen(args[i]) + 1;
        newArgs[i] = my_malloc(len * sizeof(char));
        memcpy(newArgs[i], args[i], len);
    }
    newArgs[count] = NULL;
    return newArgs;
}

static void acotatedCopy(char *dest, const char *src, size_t size) {
    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
        if (src[i] == 0) break;
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
