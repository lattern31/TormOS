#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL 0x40
#define PIT_BASE_FREQUENCY 1193180

void timer_init(uint32_t frequency);
void timer_sleep(uint32_t milliseconds);
void timer_handler();

#endif
