#ifndef GDT_H
#define GDT_H

#include "stdint.h"

struct gdt_entry {
    uint16_t limit_low;     // Младшие 16 бит предела
    uint16_t base_low;      // Младшие 16 бит базы
    uint8_t base_middle;    // Следующие 8 бит базы
    uint8_t access;         // Байт доступа
    uint8_t granularity;    // Гранулярность и старшие биты предела
    uint8_t base_high;      // Старшие 8 бит базы
} __attribute__((packed));

struct gdt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void gdt_init();

#endif
