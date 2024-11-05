#include <pipes.h>
#include <memory_manager.h>
#include <semaphore.h>
#include <lib.h>

#define BUILTIN_FDS 2
#define NAME_SIZE 64

typedef struct pipe {
    char buffer[PIPE_SIZE];
    int fds[2];
    int readPos;
    int writePos;
    int size;
    char writeSem[NAME_SIZE];
    char readSem[NAME_SIZE];
} pipe_t;

static uint16_t nextFd = BUILTIN_FDS;
static pipe_t *pipes[MAX_PIPES];

static int getIndexByFd(int fd, int pos);

void initPipes() {
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i] = NULL;
    }
}

int createPipe(int fds[2]) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i] == NULL) {
            pipes[i] = my_malloc(sizeof(pipe_t));
            if (pipes[i] == NULL) {
                return -1;
            }
            pipes[i]->readPos = 0;
            pipes[i]->writePos = 0;
            pipes[i]->size = 0;
            pipes[i]->fds[0] = nextFd++;
            fds[0] = pipes[i]->fds[0];
            pipes[i]->fds[1] = nextFd++;
            fds[1] = pipes[i]->fds[1];
            itoa(pipes[i]->fds[0], pipes[i]->writeSem);
            itoa(pipes[i]->fds[1], pipes[i]->readSem);
            strcat(pipes[i]->writeSem, "_w");
            strcat(pipes[i]->readSem, "_r");

            if(semOpen(pipes[i]->writeSem, PIPE_SIZE) == -1) {
                pipes[i] = NULL;
                return -1;
            }
            if(semOpen(pipes[i]->readSem, 0) == -1) {
                semClose(pipes[i]->writeSem);
                pipes[i] = NULL;
                return -1;
            }

            return i;
        }
    }
    return -1;
}

int writePipe(int fd, const char *buffer, int bytes) {
    if(fd < BUILTIN_FDS || fd >= nextFd) return -1;
    int index = getIndexByFd(fd, 1);
    if (index == -1) return -1;
    pipe_t *pipe = pipes[index];

    for(int i = 0; i < bytes; i++) {
        semWait(pipe->writeSem);
        pipe->buffer[pipe->writePos] = buffer[i];
        pipe->writePos = (pipe->writePos + 1) % PIPE_SIZE;
        pipe->size++;
        semPost(pipe->readSem);
    }

    return bytes;
}

int readPipe(int fd, char *buffer, int bytes) {
    if(fd < BUILTIN_FDS || fd >= nextFd) return -1;
    int index = getIndexByFd(fd, 0);
    if (index == -1) return -1;
    pipe_t *pipe = pipes[index];

    for(int i = 0; i < bytes; i++) {
        semWait(pipe->readSem);
        buffer[i] = pipe->buffer[pipe->readPos];
        pipe->readPos = (pipe->readPos + 1) % PIPE_SIZE;
        pipe->size--;
        semPost(pipe->writeSem);
    }

    return bytes;
}

void destroyPipe(int writeFd) {
    int index = getIndexByFd(writeFd, 1);
    if (index == -1) return;
    pipe_t *pipe = pipes[index];
    semClose(pipe->writeSem);
    semClose(pipe->readSem);
    my_free(pipe);
    pipes[index] = NULL;
}

static int getIndexByFd(int fd, int pos) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i] != NULL && pipes[i]->fds[pos] == fd) {
            return i;
        }
    }
    return -1;
}
