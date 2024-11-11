//
// Created by Santiago Devesa on 21/09/2024.
//

#ifndef UNTITLED_MEMORY_MANAGER_H
#define UNTITLED_MEMORY_MANAGER_H
#define MEM_SIZE 1048576
#define MIN_SIZE 32

#include <stdint.h>

typedef struct mem_info{
    uint64_t total_mem;
    uint64_t used_mem;
    uint64_t free_mem;
} mem_info_t;

void my_mem_init(void * m, uint32_t s);
void * my_malloc(uint32_t size);
void my_free(void * ptr);
mem_info_t * mem_dump();


#endif //UNTITLED_MEMORY_MANAGER_H
