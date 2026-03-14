#ifndef HEAP_H
#define HEAP_H

#include "stdint.h"

typedef struct _KHeapBlock {
    struct _KHeapBlock *next;
    uint32_t size;
} KHeapBlock;

typedef struct _KHeap {
    KHeapBlock *fblock;
} KHeap;

void KHeap_init();
void* malloc(uint32_t size);
extern KHeap kernel_heap;

#endif
