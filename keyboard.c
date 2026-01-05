#include "keyboard.h"
#include <stdint.h>

#define KEY_UP    0x01
#define KEY_DOWN  0x02

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* scancode set 1 */
static const char scancode_map[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};

/* ring buffer */
#define KBD_BUF_SIZE 128
static volatile char buf[KBD_BUF_SIZE];
static volatile int head = 0, tail = 0;

static void buf_push(char c) {
    int next = (head + 1) % KBD_BUF_SIZE;
    if (next != tail) { buf[head] = c; head = next; }
}

int keyboard_read_char(char *out) {
    if (tail == head) return 0;
    *out = buf[tail];
    tail = (tail + 1) % KBD_BUF_SIZE;
    return 1;
}

/* called from ISR */
void keyboard_isr(void) {
    uint8_t sc = inb(0x60);

    if (sc == 0x48) {        // up arrow
        buf_push(KEY_UP);
        return;
    }
    if (sc == 0x50) {        // down arrow
        buf_push(KEY_DOWN);
        return;
    }

    if (sc == 0x4B) { // left
        buf_push(KEY_LEFT); 
        return; 
    }   
    if (sc == 0x4D) { // right
        buf_push(KEY_RIGHT); 
        return; 
    }  


    if (sc & 0x80) return;     // key release
    char c = scancode_map[sc];
    if (c) buf_push(c);
}

void keyboard_init(void) {
    /* nothing yet */
}
