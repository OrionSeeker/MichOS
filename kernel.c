#include "vga.h"
#include "pit.h"
#include "pic.h"
#include "idt.h"
#include "gdt.h"
#include "keyboard.h"

#include <stdint.h>

#define INPUT_MAX 128

__attribute__((aligned(16)))
static uint8_t kernel_stack[16384];

/* ================= STRING UTILS ================= */

int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 1;
        a++; b++;
    }
    return *a != *b;
}

int starts_with(const char* str, const char* pre) {
    while (*pre) {
        if (*str++ != *pre++) return 0;
    }
    return 1;
}

/* ================= PRINT INT ================= */

void print_uint(uint32_t n) {
    char buf[16];
    int i = 0;

    if (n == 0) {
        put_char('0');
        return;
    }

    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    while (i--) {
        put_char(buf[i]);
    }
}

/* ================= SHELL ================= */

void shell_execute(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        print("Available commands:\n");
        print("  help    - show this message\n");
        print("  clear   - clear screen\n");
        print("  echo    - echo text\n");
        print("  uptime  - show uptime (seconds)\n");
        print("  sleep N - sleep N seconds\n");
    }
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
    else if (starts_with(cmd, "echo ")) {
        print(cmd + 5);
        print("\n");
    }
    else if (strcmp(cmd, "uptime") == 0) {
        print("Uptime: ");
        print_uint(pit_uptime_seconds());
        print(" seconds\n");
    }
    else if (starts_with(cmd, "sleep ")) {
        int sec = 0;
        const char* p = cmd + 6;
        while (*p >= '0' && *p <= '9') {
            sec = sec * 10 + (*p - '0');
            p++;
        }
        print("Sleeping...\n");
        pit_sleep(sec * 1000);
        print("Done.\n");
    }
    else if (cmd[0] == 0) {
        /* empty */
    }
    else {
        print("Unknown command: ");
        print(cmd);
        print("\n");
    }
}

/* ================= KERNEL MAIN ================= */

void kernel_main() {
    char input[INPUT_MAX];
    int len = 0;

    clear_screen();
    print("MichOS v0.2\n");
    print("by Michael Effendy (OrionSeeker on GitHub)\n\n");

    /* ---- CRITICAL INIT ORDER ---- */
    __asm__ volatile ("cli");

    /* setup kernel stack */
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

    print("Timer + Keyboard interrupt active\n");
    print("Type 'help' to see commands\n\n> ");

    while (1) {
        char c;
        if (keyboard_read_char(&c)) {

            if (c == '\n') {
                input[len] = 0;
                print("\n");
                shell_execute(input);
                print("> ");
                len = 0;
            }
            else if (c == '\b') {
                if (len > 0) {
                    len--;
                    backspace();
                }
            }
            else {
                if (len < INPUT_MAX - 1) {
                    input[len++] = c;
                    put_char(c);
                }
            }
        }

        __asm__ volatile ("hlt");
    }
}
