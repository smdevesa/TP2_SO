#include <programs.h>
#include <iolib.h>
#include <syscalls.h>

#define EOF -1

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