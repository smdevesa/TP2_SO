#include <commands.h>
#include <iolib.h>
#include <stringutils.h>
#include <programs.h>
#include <syscalls.h>
#include <test_util.h>

#define HELP_IDX 0
#define CLEAR_IDX 1
#define EXIT_IDX 2

#define COMMAND_SECONDARY_COLOR 0x00F5ED51
#define ERROR_PRIMARY_COLOR 0x00B63831
#define ERROR_SECONDARY_COLOR 0x00DD5E56

typedef struct command {
    char *name;
    char *description;
    uint8_t builtin;
    mainFunction function;
} command_t;

typedef struct executable_command {
    char *command;
    char *args[MAX_ARGS + 1];
    int argc;
    int fds[2];
    int pid;
} executable_command_t;

extern void _invalidOp();

static int helpCommand(int argc, char * argv[]);
static int clearCommand(int argc, char * argv[]);
static int exitCommand(int argc, char * argv[]);
static int inforegCommand(int argc, char * argv[]);
static int fillCommandAndArgs(char ** command, char * args[], char * input);
static void printError(char * command, char * message, char * usage);
static int killCommand(int argc, char * argv[]);

static command_t commands[] = {
        {"help", "Shows the available commands.", 1, &helpCommand},
        {"clear", "Clears the screen.", 1, &clearCommand},
        {"exit", "Exits the shell.", 1, &exitCommand},
        {"inforeg", "Shows the registers values.", 1, &inforegCommand},
        {"tmm", "Tests memory manager.", 0, (mainFunction)&test_mm},
        {"ts", "Tests the scheduler.", 0, (mainFunction)&test_processes},
        {"tp", "Tests priority.", 0, (mainFunction)&test_prio},
        {"ps", "Shows the process list.", 0, (mainFunction)&ps},
        {"kill", "Kills a process. Usage: kill [pid]", 1, &killCommand},
        {"tsy", "Tests the synchronization primitives. Usage: tsy [n] [use_sem]", 0, (mainFunction)&test_sync},
        {"cat", "Prints stdin as received.", 0, (mainFunction)&cat}
};

#define COMMANDS_COUNT (sizeof(commands) / sizeof(commands[0]))

static uint8_t foreground = 1;

static const char * regNames[REGS_AMOUNT] = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RSP"
};

int parseCommand(char *input) {
    if (input == NULL) {
        return INPUT_ERROR;
    }

    executable_command_t executable_commands[MAX_COMMANDS];
    for(int i=0; i<MAX_COMMANDS; i++) {
        executable_commands[i].pid = -1;
    }

    char * pipe_pos = strchr(input, '|');
    size_t executable_command_count = pipe_pos == NULL ? 1 : 2;
    if(pipe_pos) {
        *pipe_pos = 0;
        if(strchr(pipe_pos + 1, '|')) {
            printError("pipe", "Only one pipe is allowed.", NULL);
            return ERROR;
        }
    };

    for(int i=0; i<executable_command_count; i++) {
        executable_commands[i].argc = fillCommandAndArgs(&executable_commands[i].command, executable_commands[i].args, input);
        if(executable_commands[i].argc == INPUT_ERROR) {
            return INPUT_ERROR;
        }
        if(pipe_pos) input = pipe_pos + 1;
    }

    if(pipe_pos) {
        int pipefds[2];
        if(_sys_create_pipe(pipefds) == -1) {
            printError("pipe", "Error creating pipe.", NULL);
            return ERROR;
        }
        executable_commands[0].fds[1] = pipefds[1];
        executable_commands[1].fds[0] = pipefds[0];
        executable_commands[1].fds[1] = STDOUT_FD;
        executable_commands[0].fds[0] = STDIN_FD;
    }
    else {
        executable_commands[0].fds[0] = foreground ? STDIN_FD : -1;
        executable_commands[0].fds[1] = STDOUT_FD;
    }

    for(int i=0; i<executable_command_count; i++) {
        uint8_t found = 0;
        for(int j=0; j<COMMANDS_COUNT; j++) {
            if(strcmp(executable_commands[i].command, commands[j].name) == 0) {
                found = 1;
                if(commands[j].builtin) {
                    return commands[j].function(executable_commands[i].argc, executable_commands[i].args);
                }
                else {
                    executable_commands[i].pid = _sys_createProcess(commands[j].function, executable_commands[i].args, executable_commands[i].command, 0,executable_commands[i].fds);
                    if(executable_commands[i].pid == -1) {
                        return ERROR;
                    }
                }
                break;
            }
        }
        if(!found) {
            printError(executable_commands[i].command, "Command not found.", NULL);
            for(int j=0; j<executable_command_count; j++) {
                if(executable_commands[j].pid != -1) {
                    _sys_kill(executable_commands[j].pid);
                }
            }
            return ERROR;
        }
    }

    if(foreground) {
        for(int i=0; i<executable_command_count; i++) {
            if(executable_commands[i].pid != -1) {
                _sys_waitpid(executable_commands[i].pid);
            }
        }
        if(pipe_pos) {
            _sys_destroy_pipe(executable_commands[0].fds[1]);
        }
    }

    return OK;
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

    args[argsCount] = NULL;
    return argsCount;
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
