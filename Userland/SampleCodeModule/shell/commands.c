#include <commands.h>
#include <iolib.h>
#include <stringutils.h>
#include <time.h>
#include <syscalls.h>
#include <test_util.h>

#define HELP_IDX 0
#define CLEAR_IDX 1
#define EXIT_IDX 2

#define COMMAND_SECONDARY_COLOR 0x00F5ED51
#define ERROR_PRIMARY_COLOR 0x00B63831
#define ERROR_SECONDARY_COLOR 0x00DD5E56

typedef struct command {
    char * name;
    char * description;
    uint8_t builtin;
    mainFunction function;
} command_t;

extern void _invalidOp();

static int createProcessWrapper(mainFunction main, char ** argv, char * name, int fds[]);
static int helpCommand(int argc, char * argv[]);
static int clearCommand(int argc, char * argv[]);
static int exitCommand(int argc, char * argv[]);
static int inforegCommand(int argc, char * argv[]);
static int fillCommandAndArgs(char ** command, char * args[], char * input);
static void printError(char * command, char * message, char * usage);
static int psCommand(int argc, char * argv[]);
static int killCommand(int argc, char * argv[]);
static void printPsHeader();

static command_t commands[] = {
        {"help", "Shows the available commands.", 1, &helpCommand},
        {"clear", "Clears the screen.", 1, &clearCommand},
        {"exit", "Exits the shell.", 1, &exitCommand},
        {"inforeg", "Shows the registers values.", 1, &inforegCommand},
        {"tmm", "Tests memory manager.", 0, (mainFunction)&test_mm},
        {"ts", "Tests the scheduler.", 0, (mainFunction)&test_processes},
        {"tp", "Tests priority.", 0, (mainFunction)&test_prio},
        {"ps", "Shows the process list.", 1, &psCommand},
        {"kill", "Kills a process. Usage: kill [pid]", 1, &killCommand},
        {"tsy", "Tests the synchronization primitives. Usage: tsy [n] [use_sem]", 0, (mainFunction)&test_sync}
};

#define COMMANDS_COUNT (sizeof(commands) / sizeof(commands[0]))

static uint8_t foreground = 1;
static int writeFd = STDIN_FD;
static int readFd = STDOUT_FD;

static const char * regNames[REGS_AMOUNT] = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RSP"
};

int parseCommand(char *input) {
    if (input == NULL) {
        return INPUT_ERROR;
    }

    char *args[MAX_ARGS] = {0};
    char *command = NULL;
    int argsCount = fillCommandAndArgs(&command, args, input);
    if (argsCount == INPUT_ERROR) {
        return INPUT_ERROR;
    }

    for (int i = 0; i < COMMANDS_COUNT; i++) {
        if (strcmp(command, commands[i].name) == 0) {
            if (commands[i].builtin) {
                return commands[i].function(argsCount, args);
            } else {
                int fds[2] = {writeFd, readFd};
                return createProcessWrapper(commands[i].function, args, command, fds);
            }
        }
    }
    return INPUT_ERROR;
}

static int fillCommandAndArgs(char **command, char *args[], char *input) {
    int argsCount = 0;
    foreground = 1;
    writeFd = STDIN_FD;
    readFd = STDOUT_FD;
    char *current = input;

    while (*current == ' ') {
        current++;
    }

    *command = current;

    while (*current != 0) {
        if (*current == '&' && (*(current + 1) == '\0' || *(current + 1) == ' ')) {
            foreground = 0;
            *current = 0;
            break;
        }

        if (*current == ' ') {
            *current = 0;

            if (*(current + 1) != 0 && *(current + 1) != ' ' && *(current + 1) != '&') {
                args[argsCount++] = current + 1;
                if (argsCount >= MAX_ARGS) {
                    break;
                }
            }
        }
        current++;
    }

    args[argsCount] = NULL;
    return argsCount;
}

static int createProcessWrapper(mainFunction main, char ** argv, char * name, int fds[]) {
    int pid = _sys_createProcess(main, argv, name, 0, fds);
    if(pid == -1) {
        printError(name, "Error creating process.", NULL);
        return ERROR;
    }
    if(foreground) {
        _sys_waitpid(pid);
    }
    return OK;
}

static int helpCommand(int argc, char * argv[]) {
    for(int i=0; i<COMMANDS_COUNT; i++) {
        printStringColor(commands[i].name, COMMAND_SECONDARY_COLOR);
        printf(": %s\n", commands[i].description);
    }
    return OK;
}

static int clearCommand(int argc, char * argv[]) {
    clearScreen();
    return OK;
}

static int exitCommand(int argc, char * argv[]) {
    clearScreen();
    return EXIT;
}

static int inforegCommand(int argc, char * argv[]) {
    uint64_t regs[REGS_AMOUNT];
    int ok = _sys_getRegisters(regs);
    if(!ok) {
        printError("inforeg", "Registers are not updated. Use CTRL + R to update.", NULL);
        return ERROR;
    }
    for(int i=0; i<REGS_AMOUNT; i += 2) {
        printStringColor(regNames[i], COMMAND_SECONDARY_COLOR);
        printf(": %x\t", regs[i]);
        if(i < (REGS_AMOUNT - 1)) {
            printStringColor(regNames[i + 1], COMMAND_SECONDARY_COLOR);
            printf(": %x\n", regs[i + 1]);
        }
        else
            putchar('\n');
    }
    return OK;
}

static void printError(char * command, char * message, char * usage) {
    printf("%s: ", command);
    printStringColor("error: ", ERROR_SECONDARY_COLOR);
    printStringColor(message, ERROR_PRIMARY_COLOR);
    if(usage != NULL)
        printf("\nUsage: %s\n", usage);
    else
        putchar('\n');
}

static void printPsHeader() {
    printStringColor("PID; NAME; PARENT; PRIORITY; UNKILLABLE; STATUS; STACK_BASE\n", COMMAND_SECONDARY_COLOR);
}

static int psCommand(int argc, char * argv[]) {
    processInfo_t * processList = _sys_ps();
    char * statusString[] = {"READY", "BLOCKED", "RUNNING", "TERMINATED"};
    processInfo_t * current = processList;
    printPsHeader();
    while(current->pid != -1) {
        printf("%d; %s; %d; %d; %d; %s; %x\n",
               current->pid,
               current->name,
               current->parent,
               current->priority,
               current->unkillable,
               statusString[current->status],
               (uint64_t)current->stackBase);
        current++;
    }
    return OK;
}

static int killCommand(int argc, char * argv[]) {
    if(argc != 1) {
        printError("kill", "Invalid amount of arguments.", "kill [pid]");
        return ERROR;
    }
    int pid = atoi(argv[0]);
    int ret = _sys_kill(pid);
    if(ret == -1) {
        printError("kill", "Error killing process.", NULL);
        return ERROR;
    }
    return OK;
}
