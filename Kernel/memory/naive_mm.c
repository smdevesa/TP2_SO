//
// Created by Santiago Devesa on 21/09/2024.
//
#ifndef BUDDY_MM

#include "../include/memory_manager.h"
#include <stdint.h>
#include <stddef.h>

// 256MB
#define BLOCK_SIZE 4096
#define MAX_BLOCKS 1024

static void * start;
static int current;
static uint64_t used_mem;
static uint64_t size;
static void * freeBlocks[MAX_BLOCKS];
static mem_info_t mem_info;

void my_mem_init(void * m, uint32_t s) {
    start = m;
    current = 0;
    size = s;
    used_mem = 0;

    for(int i=0; i<MAX_BLOCKS; i++) {
        freeBlocks[i] = start + (i * BLOCK_SIZE);
    }
}


void * my_malloc(uint32_t blockSize) {
    if(blockSize > BLOCK_SIZE) return NULL;
    if(current < MAX_BLOCKS) {
        used_mem += BLOCK_SIZE;
        return freeBlocks[current++];
    }
    return NULL;
}

void my_free(void * ptr) {
    used_mem -= BLOCK_SIZE;
    freeBlocks[--current] = ptr;
}

mem_info_t * mem_dump(){
    mem_info.total_mem = MAX_BLOCKS * BLOCK_SIZE;
    mem_info.used_mem = used_mem;
    mem_info.free_mem = (MAX_BLOCKS * BLOCK_SIZE) - used_mem;
    return &mem_info;
}

#endif
