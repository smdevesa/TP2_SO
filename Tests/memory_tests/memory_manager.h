//
// Created by Santiago Devesa on 21/09/2024.
//

#ifndef UNTITLED_MEMORY_MANAGER_H
#define UNTITLED_MEMORY_MANAGER_H

#include <stdint.h>

void my_mem_init(void *m, uint32_t s);
void *my_malloc(uint32_t size);
void my_free(void *ptr);

#endif // UNTITLED_MEMORY_MANAGER_H
