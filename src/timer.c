#include "timer.h"
#include "ports.h"
#include "stdio.h"

static volatile uint32_t timer_ticks = 0;
static uint32_t tick_ms = 50;

void timer_handler() {
    timer_ticks++;
}

void timer_init(uint32_t frequency) {
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    outb(0x43, 0x36);
    // upper lower bytes
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor>>8) & 0xFF);
    outb(0x40, l);
    outb(0x40, h);
}

void timer_sleep(uint32_t milliseconds) {
    uint32_t ticks_needed = milliseconds / tick_ms;
    if (milliseconds % tick_ms) ticks_needed++;

    uint32_t start_ticks = timer_ticks;

    while (timer_ticks - start_ticks < ticks_needed) {
        asm volatile("hlt");
    }
}
