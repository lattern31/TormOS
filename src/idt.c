#include "idt.h"
extern void idt_load(struct idt_ptr *idtp);

struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_setgate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags)
{
    idt[interrupt].base_low = ((uint32_t)base) & 0xFFFF;
    idt[interrupt].selector = segmentDescriptor;
    idt[interrupt].zero = 0;
    idt[interrupt].type_attributes= flags;
    idt[interrupt].base_high = ((uint32_t)base >> 16) & 0xFFFF;
}

void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt[i] = (struct idt_entry){0xFFFF, 0, 0, 0, 0};
    }
    idt_load(&idtp);
};
