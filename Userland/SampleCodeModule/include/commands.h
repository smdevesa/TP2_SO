#ifndef TPE_ARQ_COMMANDS_H
#define TPE_ARQ_COMMANDS_H

#include <stddef.h>

#define MAX_COMMAND_SIZE 128
#define MAX_ARGS 5
#define MAX_COMMANDS 2

#define INPUT_ERROR -1
#define OK 0
#define EXIT 1
#define ERROR 2

/**
 * @brief Parses the command and executes it.
 * @param input the command to parse.
 * @return 0 if the command was executed successfully, 1 if the command was not found.
 */
int parseCommand(char * input);

#endif //TPE_ARQ_COMMANDS_H
