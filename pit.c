#include "pit.h"

/* Correct port I/O helpers */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(val), "Nd"(port)
    );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

static volatile uint32_t pit_ticks = 0;
static uint32_t pit_freq = 100;

void pit_init(uint32_t freq) {
    pit_freq = freq;
    uint32_t divisor = 1193182 / freq;

    outb(0x43, 0x36);                 // channel 0, mode 3
    outb(0x40, divisor & 0xFF);       // low byte
    outb(0x40, (divisor >> 8) & 0xFF);// high byte
}

/* Called from IRQ0 ISR */
void pit_tick(void) {
    pit_ticks++;
}

void pit_sleep(uint32_t ms) {
    uint32_t target = pit_ticks + (ms * pit_freq) / 1000;

    while (pit_ticks < target) {
        __asm__ volatile ("hlt");
    }
}

uint32_t pit_uptime_seconds(void) {
    return pit_ticks / pit_freq;
}
