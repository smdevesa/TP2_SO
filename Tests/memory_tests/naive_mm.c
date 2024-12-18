// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// Created by Santiago Devesa on 21/09/2024.
//

#include "memory_manager.h"
#include <stddef.h>
#include <stdint.h>

#define BLOCK_SIZE 1024
#define MAX_BLOCKS 128

static void *start;
static int current;
static uint64_t size;
static void *freeBlocks[MAX_BLOCKS];

void my_mem_init(void *m, uint32_t s) {
	start = m;
	current = 0;
	for (int i = 0; i < MAX_BLOCKS; i++) {
		freeBlocks[i] = start + (i * BLOCK_SIZE);
	}
	size = s;
}

void *my_malloc(uint32_t size) {
	if (size > BLOCK_SIZE)
		return NULL;
	if (current < MAX_BLOCKS) {
		return freeBlocks[current++];
	}
	return NULL;
}

void my_free(void *ptr) {
	freeBlocks[--current] = ptr;
}
