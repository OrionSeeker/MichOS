#include "vga.h"
#include "pit.h"
#include "pic.h"
#include "idt.h"
#include "gdt.h"
#include "keyboard.h"
#include "shell.h"

#include <stdint.h>

__attribute__((aligned(16)))
static uint8_t kernel_stack[16384];

void kernel_main() {
    clear_screen();
    print("MichOS v0.3\n");
    print("by Michael Effendy (OrionSeeker on GitHub)\n\n");

    print("Type 'help' to see commands\n\n");


    __asm__ volatile ("cli");
    __asm__ volatile (
        "mov %0, %%esp"
        :
        : "r"(kernel_stack + sizeof(kernel_stack))
    );

    gdt_flush();
    pic_remap();
    idt_init();
    pit_init(100);
    keyboard_init();

    __asm__ volatile ("sti");

    shell_init();

    while (1) {
        char c;
        if (keyboard_read_char(&c)) {
            shell_handle_char(c);
        }
        __asm__ volatile ("hlt");
    }
}
