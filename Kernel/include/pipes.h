#ifndef TP2_SO_PIPES_H
#define TP2_SO_PIPES_H

#define MAX_PIPES 64
#define PIPE_SIZE 1024

void initPipes();
int createPipe(int fds[2]);
int writePipe(int fd, const char *buffer, int bytes);
int readPipe(int fd, char *buffer, int bytes);
void destroyPipe(int writeFd);
void send_pipe_eof(int fd);

#endif // TP2_SO_PIPES_H
