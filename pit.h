#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void pit_init(uint32_t freq);
void pit_tick(void);
void pit_sleep(uint32_t ms);
uint32_t pit_uptime_seconds(void);

#endif
