#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEY_UP    0x01
#define KEY_DOWN  0x02
#define KEY_LEFT   0x03
#define KEY_RIGHT  0x04

void keyboard_init(void);
int  keyboard_read_char(char *out); // non-blocking

#endif
