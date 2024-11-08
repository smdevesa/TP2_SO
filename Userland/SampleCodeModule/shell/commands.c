#include <commands.h>
#include <iolib.h>
#include <stringutils.h>
#include <time.h>
#include <syscalls.h>
#include <eliminator.h>
#include <philosophers.h>
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
    int (*function)(int argc, char * argv[]);
} command_t;

extern void _invalidOp();
static int helpCommand(int argc, char * argv[]);
static int clearCommand(int argc, char * argv[]);
static int exitCommand(int argc, char * argv[]);
static int dateCommand(int argc, char * argv[]);
static int fontscaleCommand(int argc, char * argv[]);
static int inforegCommand(int argc, char * argv[]);
static int eliminatorCommand(int argc, char * argv[]);
static int fillCommandAndArgs(char ** command, char * args[], char * input);
static void printError(char * command, char * message, char * usage);
static int exceptionCommand(int argc, char * argv[]);
static int testSchedulerCommand(int argc, char * argv[]);
static int testPriorityCommand(int argc, char * argv[]);
static int psCommand(int argc, char * argv[]);
static int killCommand(int argc, char * argv[]);
static int testMemoryManagerCommand(int argc, char * argv[]);
static void printPsHeader();
static int testSyncCommand(int argc, char * argv[]);
static int phyloCommand(int argc, char * argv[]);

static command_t commands[] = {
        {"help", "Shows the available commands.", 1, &helpCommand},
        {"clear", "Clears the screen.", 1, &clearCommand},
        {"exit", "Exits the shell.", 1, &exitCommand},
        {"date", "Shows the current date and time.", 1, &dateCommand},
        {"fontscale", "Sets the font scale. Usage: fontscale [1, 2, 3]", 1, &fontscaleCommand},
        {"inforeg", "Shows the registers values.", 1, &inforegCommand},
        {"eliminator", "Starts the Eliminator game.", 1, &eliminatorCommand},
        {"exception", "To test exceptions. Usage: exception [zero, invalidOpcode]", 1, &exceptionCommand},
        {"tmm", "Tests memory manager.", 0, &testMemoryManagerCommand},
        {"ts", "Tests the scheduler.", 0, &testSchedulerCommand},
        {"tp", "Tests priority.", 0, &testPriorityCommand},
        {"ps", "Shows the process list.", 1, &psCommand},
        {"kill", "Kills a process. Usage: kill [pid]", 1, &killCommand},
        {"tsy", "Tests the synchronization primitives. Usage: tsy [n] [use_sem]", 0, &testSyncCommand},
        {"phylo", "Starts the philosophers program.", 0, &phyloCommand}
};

#define COMMANDS_COUNT (sizeof(commands) / sizeof(commands[0]))

// Default scale
static int scale = 1;
static uint8_t foreground = 1;

static const char * regNames[REGS_AMOUNT] = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RSP"
};

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

static int dateCommand(int argc, char * argv[]) {
    printf("%d/%d/%d %d:%d:%d\n", getDay(), getMonth(), getYear(), getHours(), getMinutes(), getSeconds());
    return OK;
}

static int fontscaleCommand(int argc, char * argv[]) {
    if(argc != 1 || argv[0] == NULL) {
        printError("fontscale", "Invalid amount of arguments.", "fontscale [1, 2, 3]");
        return ERROR;
    }
    if(argv[0][0] < '1' || argv[0][0] > '3') {
        printError("fontscale", "Invalid scale.", "fontscale [1, 2, 3]");
        return ERROR;
    }
    int newScale = atoi(argv[0]);
    scale = newScale;
    setFontScale(scale);
    clearCommand(argc, argv);
    return OK;
}

static int inforegCommand(int argc, char * argv[]) {
    uint64_t regs[REGS_AMOUNT];
    int ok = _sys_getRegisters(regs);
    if(!ok) {
        printError("inforeg", "Registers are not updated. Use CTRL + R to update.", NULL);
        return ERROR;
    }
    char changed = 0;
    if(scale == 3) {
        changed = 1;
        setFontScale(2);
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
    if(changed) {
        setFontScale(3);
    }
    return OK;
}

static int eliminatorCommand(int argc, char * argv[]) {
    eliminator();
    setFontScale(scale);
    return OK;
}

static int exceptionCommand(int argc, char * argv[]){
    if(argc != 1 || argv[0] == NULL) {
        printError("exception", "Invalid amount of arguments.", "exception [zero, invalidOpcode]");
        return ERROR;
    }
    if(strcmp(argv[0], "zero") == 0) {
        int a = 1;
        int b = 0;
        int c = a / b;
        printf("c: %d\n", c);
    }
    else if(strcmp(argv[0], "invalidOpcode") == 0) {
        _invalidOp();
    }
    else {
        printError("exception", "Invalid exception type.", "exception [zero, invalidOpcode]");
        return ERROR;
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
                int pid = commands[i].function(argsCount, args);
                if (pid != -1) {
                    if (foreground) {
                        _sys_waitpid(pid);
                    }
                    return OK;
                }
                return ERROR;
            }
        }
    }
    return INPUT_ERROR;
}

static int fillCommandAndArgs(char **command, char *args[], char *input) {
    int argsCount = 0;
    foreground = 1;
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

    return argsCount;
}



static int testSchedulerCommand(int argc, char *argv[]) {
    if(argc != 1) {
        printError("test_scheduler", "Invalid amount of arguments.", "ts [processes]");
        return ERROR;
    }
    int processes = atoi(argv[0]);
    if(processes <= 0 || processes > 25) {
        printError("test_scheduler", "Invalid amount of processes. Process count must be in [1-25]", NULL);
        return ERROR;
    }
    char * args[] = {argv[0], NULL};
    int pid = _sys_createProcess((mainFunction)&test_processes, args, "test_processes", 1, 0);
    if(pid == -1) {
        printError("test_scheduler", "Error creating process.", NULL);
        return ERROR;
    }
    return pid;
}

static int testPriorityCommand(int argc, char *argv[]) {
    char * args[] = {NULL};
    int pid = _sys_createProcess((mainFunction)&test_prio, args, "test_priority", 1, 0);
    if(pid == -1) {
        printError("test_priority", "Error creating process.", NULL);
        return -1;
    }
    return pid;
}

static void printPsHeader() {
    printStringColor("PID; NAME; PARENT; PRIORITY; UNKILLABLE; STATUS; STACK_BASE\n", COMMAND_SECONDARY_COLOR);
}

static int psCommand(int argc, char * argv[]) {
    int oldScale = scale;
    setFontScale(1);
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
    if(oldScale != 1) {
        setFontScale(oldScale);
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

static int testMemoryManagerCommand(int argc, char * argv[]) {
    if(argc != 1) {
        printError("test_memory_manager", "Invalid amount of arguments.", "tmm [mem_amount]");
        return -1;
    }
    // const because of naive mm
    char * args[] = {"524288", NULL};
    int pid = _sys_createProcess((mainFunction)&test_mm, args, "test_memory_manager", 1, 0);
    if(pid == -1) {
        printError("test_memory_manager", "Error creating process.", NULL);
        return -1;
    }
    return pid;
}

static int testSyncCommand(int argc, char * argv[]) {
    if(argc != 2) {
        printError("test_sync", "Invalid amount of arguments.", "tsy [n] [use_sem]");
        return -1;
    }
    char * args[] = {argv[0], argv[1], NULL};
    int pid = _sys_createProcess((mainFunction)&test_sync, args, "test_sync", 1, 0);
    if(pid == -1) {
        printError("test_sync", "Error creating process.", NULL);
        return -1;
    }
    return pid;
}

static int phyloCommand(int argc, char * argv[]) {
    philosopherProgram(argc, argv);
    return OK;
}