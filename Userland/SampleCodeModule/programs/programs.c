#include <programs.h>
#include <iolib.h>
#include <syscalls.h>

int cat(int argc, char **argv) {
    if(argc > 1) {
        printf("cat: too many arguments\n");
        return 1;
    }
    if(argc == 1) {
        printf("%s", argv[0]);
        return 0;
    }

    char c;
    while(_sys_read(STDIN_FD, &c, 1) > 0) {
        printf("%c", c);
    }
    return 0;
}