#include "shell.h"
#include "vga.h"
#include "pit.h"

#define INPUT_MAX 128
#define HISTORY_SIZE 10

static char input[INPUT_MAX];
static int len = 0;

static const char* commands[] = {
    "help",
    "clear",
    "uptime",
    "sleep"
};

#define CMD_COUNT (sizeof(commands)/sizeof(commands[0]))


/* history */
static char history[HISTORY_SIZE][INPUT_MAX];
static int history_count = 0;
static int history_index = -1;

/* utils */
static int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 1;
        a++; b++;
    }
    return *a != *b;
}

static int starts_with(const char* s, const char* p) {
    while (*p) {
        if (*s++ != *p++) return 0;
    }
    return 1;
}

static void print_uint(unsigned int n) {
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
    while (i--) put_char(buf[i]);
}

/* history helpers */
static void history_add(void) {
    if (len == 0) return;

    int idx = history_count % HISTORY_SIZE;
    for (int i = 0; i < len; i++) {
        history[idx][i] = input[i];
        history[idx][i + 1] = 0;
    }
    history_count++;
}

static void history_show(int index) {
    clear_line();
    print("> ");

    int real = index % HISTORY_SIZE;
    len = 0;
    while (history[real][len]) {
        input[len] = history[real][len];
        put_char(input[len]);
        len++;
    }
    input[len] = 0;
}

static void autocomplete(void) {
    if (len == 0) return;

    int match = -1;
    int matches = 0;

    for (int i = 0; i < CMD_COUNT; i++) {
        const char* cmd = commands[i];
        int j = 0;

        while (j < len && cmd[j] && input[j] == cmd[j])
            j++;

        if (j == len) {
            match = i;
            matches++;
        }
    }

    /* satu match = autocomplete */
    if (matches == 1) {
        const char* cmd = commands[match];
        int j = len;

        while (cmd[j]) {
            input[len++] = cmd[j];
            put_char(cmd[j]);
            j++;
        }
    }
    /* banyak match = tampilkan */
    else if (matches > 1) {
        print("\n");
        for (int i = 0; i < CMD_COUNT; i++) {
            const char* cmd = commands[i];
            int j = 0;

            while (j < len && cmd[j] && input[j] == cmd[j])
                j++;

            if (j == len) {
                print(cmd);
                print("  ");
            }
        }
        print("\n> ");
        for (int i = 0; i < len; i++)
            put_char(input[i]);
    }
}


/* command exec */
static void shell_execute(void) {
    input[len] = 0;

    if (strcmp(input, "help") == 0) {
        print("Available commands:\n");
        print("  help    - show this message\n");
        print("  clear   - clear screen\n");
        print("  echo    - echo text\n");
        print("  uptime  - show uptime (seconds)\n");
        print("  sleep N - sleep N seconds\n\n");
    }
    else if (strcmp(input, "clear") == 0) {
        clear_screen();
    }
    else if (strcmp(input, "uptime") == 0) {
        print("Uptime: ");
        print_uint(pit_uptime_seconds());
        print(" sec\n");
    }
    else if (starts_with(input, "sleep ")) {
        int sec = 0;
        char* p = input + 6;
        while (*p >= '0' && *p <= '9') {
            sec = sec * 10 + (*p - '0');
            p++;
        }
        print("Sleeping...\n");
        pit_sleep(sec * 1000);
        print("Done.\n");
    }
    else if (len > 0) {
        print("Unknown command\n");
    }
}

void shell_init(void) {
    print("> ");
}

void shell_handle_char(char c) {
    if (c == '\n') {
        print("\n");
        history_add();
        history_index = -1;
        shell_execute();
        len = 0;
        print("> ");
    }
    else if (c == '\b') {
        if (len > 0) {
            len--;
            backspace();
        }
    }
    
    else if (c == 0x01) { /* KEY_UP */
        if (history_count > 0) {
            if (history_index < 0)
                history_index = history_count - 1;
            else if (history_index > 0)
                history_index--;
            history_show(history_index);
        }
    }
    else if (c == 0x02) { /* KEY_DOWN */
        if (history_index >= 0) {
            history_index++;
            if (history_index >= history_count) {
                history_index = -1;
                clear_line();
                print("> ");
                len = 0;
            } else {
                history_show(history_index);
            }
        }
    }
    else if (c == '\t') {
        autocomplete();
    }

    else {
        if (len < INPUT_MAX - 1) {
            input[len++] = c;
            put_char(c);
        }
    }
}