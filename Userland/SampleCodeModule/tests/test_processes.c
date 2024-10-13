#include "../include/iolib.h"
#include "../include/test_syscall.h"
#include "../include/test_util.h"
#include "../include/syscalls.h"

enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

int64_t test_processes(int argc, char *argv[]) {
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {0, NULL};

    if (argc != 1)
        return -1;

    if ((max_processes = satoi(argv[0])) <= 0)
        return -1;

    p_rq p_rqs[max_processes];
    int count = 0;

    while (1) {

        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++) {
            p_rqs[rq].pid = my_create_process((mainFunction)&endless_loop, argvAux, "endless_loop", 1, 0);
            if (p_rqs[rq].pid == -1) {
                printf("test_processes: ERROR creating process, it: %d\n", count);
                return -1;
            } else {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {

            for (rq = 0; rq < max_processes; rq++) {
                action = GetUniform(100) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
                            if (my_kill(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR killing process, it: %d\n", count);
                                return -1;
                            }
                            p_rqs[rq].state = KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING) {
                            if (my_block(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR blocking process\n");
                                return -1;
                            }
                            p_rqs[rq].state = BLOCKED;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
                    if (my_unblock(p_rqs[rq].pid) == -1) {
                        printf("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }
        count++;
    }
}