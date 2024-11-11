//
// Created by Tizifuchi12 on 11/11/2024.
//

#ifdef BUDDY_MM

#include <memory_manager.h>
#include <memoryPositions.h>
#include <stdint.h>
#include <stddef.h>

typedef struct buddy_node{
    struct buddy_node *left;
    struct buddy_node *right;
    void * address;
    uint64_t size;
    uint32_t status;
    uint32_t index;
}buddy_node_t;

typedef enum {
    EMPTY = 0,
    FULL,
    SPLIT
} status_t;

typedef buddy_node_t * node_t;

#define IS_POWER_OF_2(x) (!((x)&((x)-1)))

static void * malloc_rec(node_t node, uint32_t block_size);
static uint8_t has_not_descendant(node_t node);
static void free_rec(node_t node, void * ptr);
static void set_node_state(node_t node);
static void set_descendants(node_t node);
static uint32_t align(uint32_t size);

static node_t root;
static uint64_t allocated_mem = 0;
static mem_info_t mem_info;

void my_mem_init(void *m, uint32_t s) {
    root = (node_t) MEMORY_MANAGER_ADDRESS;
    root->address = (void *) m;
    root->index = 0;
    root->size = s;
    root->status = EMPTY;
    root->left = NULL;
}

void * my_malloc(uint32_t blockSize) {
    if(blockSize < MIN_SIZE)
        blockSize = MIN_SIZE;
    else if(blockSize > root->size) {
        return NULL;
    }
    if(!IS_POWER_OF_2(blockSize)) {
        blockSize = align(blockSize);
    }
    return malloc_rec(root, blockSize);
}

static void * malloc_rec(node_t node, uint32_t block_size) {
    if (node->status == FULL) {
        return NULL;
    }
    if(node->left != NULL || node->right != NULL){
        void * aux = malloc_rec(node->left, block_size);
        if(aux == NULL) {
            return malloc_rec(node->right, block_size);
        }
        set_node_state(node);
        return aux;
    }
    else{
        if (block_size > node->size) {
            return NULL;
        }
        if ((node->size / 2) >= block_size) {
            set_descendants(node);
            void *to_return = malloc_rec(node->left, block_size);
            set_node_state(node);
            return to_return;
        }
        node->status = FULL;
        allocated_mem += block_size;
        return node->address;
    }
}

static void set_node_state(node_t node) {
    if(has_not_descendant(node)){
        node->status = EMPTY;
    }
    else if(node->left->status == FULL && node->right->status == FULL){
        node->status = FULL;
    }
    else if (node->left->status == FULL || node->right->status == FULL || node->left->status == SPLIT || node->right->status == SPLIT) {
        node->status = SPLIT;
    } else {
        node->status = EMPTY;
    }
}

static uint8_t has_not_descendant(node_t node) {
    return (node->left == NULL || node->right == NULL);
}


static void set_descendants(node_t node) {
    uint32_t parent_index = node->index;
    uint32_t left_index = parent_index * 2 + 1;
    uint64_t descendant_size = ((uint64_t)(node->size) / 2);
    node->left = node + left_index;

    if ((uint64_t) node->left >= LIMIT) {
        return;
    }
    node->left->index = left_index;
    node->left->size = descendant_size;
    node->left->address = node->address;
    node->left->status = EMPTY;

    unsigned int right_index = left_index + 1;
    node->right = node + right_index;
    if ((uint64_t) node->right >= LIMIT) {
        return;
    }
    node->right->index = right_index;
    node->right->size = descendant_size;
    node->right->address = (void *) ((uint64_t)(node->address) + descendant_size);
    node->right->status = EMPTY;
}

void my_free(void * ptr){
    free_rec(root, ptr);
}

static void free_rec(node_t node, void * ptr){
    if(node == NULL){
        return;
    }
    if (node->left != NULL || node->right != NULL){
        if (node->right != 0 && (uint64_t) node->right->address > (uint64_t) ptr){
            free_rec(node->left, ptr);
        }
        else{
            free_rec(node->right, ptr);
        }
        set_node_state(node);
        if (node->status == EMPTY){
            node->right = NULL;
            node->left = NULL;
        }
    }
    else if (node->status == FULL) {
        if (node->address == ptr) {
            node->status = EMPTY;
            allocated_mem -= node->size;
        }
    }
    return;
}

mem_info_t * mem_dump(){
    mem_info.total_mem = MEM_SIZE;
    mem_info.used_mem = allocated_mem;
    mem_info.free_mem = MEM_SIZE - allocated_mem;
    return &mem_info;
}

static uint32_t align(uint32_t size){
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size + 1;
}

#endif