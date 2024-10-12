//
// Created by Santiago Devesa on 21/09/2024.
//

#include "../include/memory_manager.h"
#include <stdint.h>
#include <stddef.h>
#include <syscall_lib.h>

#define BLOCK_SIZE 4096
#define MAX_BLOCKS 256

static void * start;
static int current;
static uint64_t size;
static void * freeBlocks[MAX_BLOCKS];

void my_mem_init(void * m, uint32_t s) {
    start = m;
    current = 0;
    size = s;

    for(int i=0; i<MAX_BLOCKS; i++) {
        freeBlocks[i] = start + (i * BLOCK_SIZE);
    }
}


void * my_malloc(uint32_t size) {
    if(size > BLOCK_SIZE) return NULL;
    if(current < MAX_BLOCKS) {
        return freeBlocks[current++];
    }
    sys_write(1, "Me quede sin memoria\n", strlen("Me quede sin memoria\n"), 0x00FFFFFF);
    return NULL;
}

void my_free(void * ptr) {
    freeBlocks[--current] = ptr;
}
