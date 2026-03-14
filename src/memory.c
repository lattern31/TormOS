#include "memory.h"
#include "stdio.h"

void* memcpy(void *dest, const void *src, size_t n) {
    char *d = (char*)dest;
    const char *s = (const char*)src;
    
    // Простейшая реализация
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dest;
}
