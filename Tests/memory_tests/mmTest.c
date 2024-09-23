//
// Created by Santiago Devesa on 21/09/2024.
//

#include "test_util.h"
#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
    void *address;
    uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]);

int main(void) {
    char *args[] = {"131072"};
    return test_mm(1, args);
}

uint64_t test_mm(uint64_t argc, char *argv[]) {

    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;

    if (argc != 1)
        return -1;

    if ((max_memory = satoi(argv[0])) <= 0)
        return -1;

    void *start = malloc(max_memory);
    my_mem_init(start, max_memory);

    while (1) {
        rq = 0;
        total = 0;

        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < max_memory) {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = my_malloc(mm_rqs[rq].size);

            if (mm_rqs[rq].address) {
                total += mm_rqs[rq].size;
                rq++;
            }
        }

        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                memset(mm_rqs[i].address, i, mm_rqs[i].size);

        // Check
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
                    printf("test_mm ERROR");
                    return -1;
                }

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                my_free(mm_rqs[i].address);
    }
}