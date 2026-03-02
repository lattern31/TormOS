#ifndef IDT_H
#define IDT_H

#include "stdint.h"

struct idt_entry {
   uint16_t base_low;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  zero;            // unused, set to 0
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t base_high;        // offset bits 16..31
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_init();
void idt_setgate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags);

#endif
