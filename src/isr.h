#ifndef ISR_H
#define ISR_H

#include "stdint.h"

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;
    uint32_t interrupt, error;
    uint32_t eip, cs, eflags, esp, ss;
} __attribute((packed)) Registers; 

void ISR_init();
void ISR_Handler(Registers* regs);
#endif
