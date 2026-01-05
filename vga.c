#include "vga.h"

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define COLOR 0x0F

static unsigned short* vga = (unsigned short*)VGA_ADDRESS;
static int row = 0;
static int col = 0;

void put_char(char c) {
    if (c == '\n') {
        col = 0;
        row++;
        if (row >= VGA_HEIGHT) row = 0;
        return;
    }

    vga[row * VGA_WIDTH + col] = (COLOR << 8) | c;
    col++;

    if (col >= VGA_WIDTH) {
        col = 0;
        row++;
        if (row >= VGA_HEIGHT) row = 0;
    }
}

void backspace() {
    if (col == 0 && row == 0) return;

    if (col == 0) {
        row--;
        col = VGA_WIDTH - 1;
    } else {
        col--;
    }

    vga[row * VGA_WIDTH + col] = (COLOR << 8) | ' ';
}

void print(const char* str) {
    while (*str) {
        put_char(*str++);
    }
}

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (COLOR << 8) | ' ';
    }
    row = col = 0;
}
