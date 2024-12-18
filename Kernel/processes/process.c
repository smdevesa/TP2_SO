// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <memory_manager.h>
#include <process.h>
#include <scheduler.h>
#include <stddef.h>
#include <syscall_lib.h>

#define STACK_SIZE 4096
#define MAX_NAME_LENGTH 256

// create a fake iretq frame
extern void *_initialize_stack_frame(void (*wrapper)(mainFunction, char **),
									 mainFunction main, void *stackEnd,
									 void *argv);
static int argsLen(char **array);
// calls the main function of the process and then kills it
void processCaller(mainFunction main, char **args);
static char **allocArgs(char **args);
static void freeArgs(char **args);

process_t *createProcessStructure(uint16_t pid, uint16_t parentPid,
								  uint16_t waitingForPid, mainFunction main,
								  char **argv, char *name, uint8_t priority,
								  uint8_t unkillable, uint16_t writeFd,
								  uint16_t readFd) {
	process_t *p = my_malloc(sizeof(process_t));
	if (p == NULL)
		return NULL;

	p->pid = pid;
	p->parentPid = parentPid;
	p->waitingForPid = waitingForPid;
	p->priority = priority;
	p->remainingQuantum = priority;
	p->unkillable = unkillable;
	p->status = READY;
	p->writeFd = writeFd;
	p->readFd = readFd;
	p->stackBase = my_malloc(STACK_SIZE);
	if (p->stackBase == NULL) {
		my_free(p);
		return NULL;
	}
	p->stackPos = p->stackBase + STACK_SIZE;
	p->argv = allocArgs(argv);
	if (p->argv == NULL) {
		my_free(p->stackBase);
		my_free(p);
		return NULL;
	}
	strncpy(p->name, name, MAX_NAME_LENGTH - 1);
	p->name[MAX_NAME_LENGTH - 1] = 0;
	p->stackPos = _initialize_stack_frame(&processCaller, main, p->stackPos,
										  (void *) p->argv);

	return p;
}

static void freeArgs(char **args) {
	if (args == NULL)
		return;
	for (int i = 0; args[i] != NULL; i++) {
		my_free(args[i]);
	}
	my_free(args);
}

void freeProcessStructure(process_t *p) {
	if (p == NULL)
		return;
	freeArgs(p->argv);
	my_free(p->stackBase);
	my_free(p);
}

static char **allocArgs(char **args) {
	if (args == NULL || args[0] == NULL) {
		char **newArgs = my_malloc(sizeof(char *));
		if (newArgs == NULL)
			return NULL;
		newArgs[0] = NULL;
		return newArgs;
	}

	int count = argsLen(args);
	char **newArgs = my_malloc((count + 1) * sizeof(char *));
	for (int i = 0; i < count; i++) {
		int len = strlen(args[i]) + 1;
		newArgs[i] = my_malloc(len * sizeof(char));
		memcpy(newArgs[i], args[i], len);
	}
	newArgs[count] = NULL;
	return newArgs;
}

void processCaller(mainFunction main, char **args) {
	int argc = argsLen(args);
	int64_t retValue = main(argc, args);
	sys_exit(retValue);
}

static int argsLen(char **array) {
	int i = 0;
	while (array[i] != NULL)
		i++;
	return i;
}
