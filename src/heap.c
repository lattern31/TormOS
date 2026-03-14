#include "heap.h"
#include "vga/vga.h"
#include "common.h"

KHeap kernel_heap;

void KHeap_init() {
    extern uint32_t heap_start;
    KHeapBlock *new_block = (KHeapBlock*)&heap_start;
    new_block->next = INVALID_PTR;
    new_block->size = sizeof(KHeapBlock);
    kernel_heap.fblock = new_block;
};

void* malloc(uint32_t size) {
    KHeapBlock *fblock = kernel_heap.fblock;
    
    KHeapBlock *new_block = (KHeapBlock*)((uint32_t)fblock + fblock->size);
    new_block->next = fblock;
    new_block->size = size + sizeof(KHeapBlock);
    kernel_heap.fblock = new_block;
    return (void*)((uint32_t)new_block + sizeof(KHeapBlock));
};
