#include <programs.h>
#include <iolib.h>
#include <syscalls.h>

#define EOF -1
#define PS_SECONDARY_COLOR 0x00F5ED51

static void print_ps_header();

int cat(int argc, char **argv) {
    if(argc != 0) {
        printf("cat: Invalid amount of arguments.\n");
        return -1;
    }

    char c;
    while((c = getchar()) != EOF) {
        if(c) putchar(c);
    }

    return 0;
}

int ps(int argc, char **argv) {
    if(argc != 0) {
        printf("ps: Invalid amount of arguments.\n");
        return -1;
    }

    processInfo_t * process_list = _sys_ps();
    char * status_string[] = {"READY", "BLOCKED", "RUNNING", "TERMINATED"};
    processInfo_t * current = process_list;
    print_ps_header();
    while(current->pid != -1) {
        printf("%d; %s; %d; %d; %d; %s; %x\n",
               current->pid,
               current->name,
               current->parent,
               current->priority,
               current->unkillable,
               status_string[current->status],
               (uint64_t)current->stackBase);
        current++;
    }
    return 0;
}

static void print_ps_header() {
    printStringColor("PID; NAME; PARENT; PRIORITY; UNKILLABLE; STATUS; STACK_BASE\n", PS_SECONDARY_COLOR);
}