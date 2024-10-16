//
// Created by Santiago Devesa on 21/09/2024.
//

#include "../include/memory_manager.h"
#include <stdint.h>
#include <stddef.h>

// 256MB
#define BLOCK_SIZE 4096
#define MAX_BLOCKS 65536

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


void * my_malloc(uint32_t blockSize) {

    if(blockSize > BLOCK_SIZE) return NULL;
    if(current < MAX_BLOCKS) {
        return freeBlocks[current++];
    }
    return NULL;
}

void my_free(void * ptr) {
    freeBlocks[--current] = ptr;
}
