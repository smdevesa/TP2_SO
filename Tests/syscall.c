#include <stdint.h>
#include "../Kernel/include/scheduler.h"

int64_t my_getpid() {
  return 0;
}

int64_t my_create_process(char *name, uint64_t argc, char *argv[], mainFunction main) {
      return addProcess(main, argv, name, 1,0);
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
  return 0;
}

int64_t my_kill(uint64_t pid) {
  return killProcess(pid, 0) ;
}

int64_t my_block(uint64_t pid) {
  return blockProcess(pid);
}

int64_t my_unblock(uint64_t pid) {
  return unblockProcess(pid);
}

int64_t my_sem_open(char *sem_id, uint64_t initialValue) {
  return 0;
}

int64_t my_sem_wait(char *sem_id) {
  return 0;
}

int64_t my_sem_post(char *sem_id) {
  return 0;
}

int64_t my_sem_close(char *sem_id) {
  return 0;
}

int64_t my_yield() {
   yield();
   return 0;
}

int64_t my_wait(int64_t pid) {
  return 0;
}
