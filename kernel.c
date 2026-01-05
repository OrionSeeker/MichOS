#include "vga.h"

#define INPUT_MAX 128

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* scancode set 1 */
static const char scancode_map[128] = {
    0, 27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};

/* string utils (belum punya libc) */
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

void shell_execute(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        print("Available commands:\n");
        print("  help  - show this message\n");
        print("  clear - clear screen\n");
        print("  echo  - echo text\n");
    }
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
    else if (starts_with(cmd, "echo ")) {
        print(cmd + 5);
        print("\n");
    }
    else if (cmd[0] == 0) {
        /* empty command, do nothing for now */
    }
    else {
        print("Unknown command: ");
        print(cmd);
        print("\n");
    }
}

void kernel_main() {
    char input[INPUT_MAX];
    int len = 0;

    clear_screen();
    print("MichOS v0.1\n");
    print("This OS is developed by Michael Effendy");
    print(" (OrionSeeker on Github)\n");
    print("Type 'help' to see commands\n\n> ");

    while (1) {
        if (inb(0x64) & 1) {
            unsigned char sc = inb(0x60);
            if (sc & 0x80) continue;

            char c = scancode_map[sc];
            if (!c) continue;

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
    }
}
