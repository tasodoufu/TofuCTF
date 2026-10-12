#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SLOT_COUNT 8
#define CHUNK_SIZE 0x60

struct fake_fastbin_chunk {
    size_t previous_size;
    size_t size;
    void (*callback)(void);
};

/* The callback is deliberately preceded by a valid fastbin-size header. */
static struct fake_fastbin_chunk callback_target = {
    .previous_size = 0,
    .size = 0x71,
    .callback = NULL,
};

static void *slots[SLOT_COUNT];
static unsigned char in_use[SLOT_COUNT];

__attribute__((noinline, used, noreturn)) static void reveal_recipe(void) {
    char flag[128];
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        _exit(1);
    }
    ssize_t length = read(fd, flag, sizeof(flag));
    if (length > 0) {
        (void)write(STDOUT_FILENO, flag, (size_t)length);
    }
    close(fd);
    _exit(0);
}

static void discard_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static int hex_value(int ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

static int decode_hex(const char *text, unsigned char *out, size_t capacity) {
    size_t length = strlen(text);
    if (length == 0 || (length & 1U) != 0 || length / 2 > capacity) {
        return -1;
    }
    for (size_t index = 0; index < length / 2; ++index) {
        int high = hex_value((unsigned char)text[index * 2]);
        int low = hex_value((unsigned char)text[index * 2 + 1]);
        if (high < 0 || low < 0) return -1;
        out[index] = (unsigned char)((high << 4) | low);
    }
    return (int)(length / 2);
}

static void show_menu(void) {
    puts("== Fastbin Falafel ==");
    puts("The falafel counter reuses returned trays without checking the fastbin list.");
    puts("Duplicate a tray, redirect its next pointer, and replace the serving callback.");
    printf("Callback slot: %p\n", (void *)&callback_target.callback);
    puts("1) allocate  2) free  3) edit  4) serve  5) quit");
    fputs("choice> ", stdout);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    show_menu();

    for (;;) {
        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            return 0;
        }
        discard_line();

        if (choice == 1) {
            int index = -1;
            for (int candidate = 0; candidate < SLOT_COUNT; ++candidate) {
                if (!in_use[candidate]) {
                    index = candidate;
                    break;
                }
            }
            if (index < 0) {
                puts("The tray rack is full.");
            } else {
                slots[index] = malloc(CHUNK_SIZE);
                if (slots[index] == NULL) {
                    puts("Allocation failed.");
                    return 1;
                }
                in_use[index] = 1;
                printf("slot %d at %p\n", index, slots[index]);
            }
        } else if (choice == 2) {
            int index = -1;
            printf("index> ");
            if (scanf("%d", &index) != 1) return 0;
            discard_line();
            if (index < 0 || index >= SLOT_COUNT || slots[index] == NULL) {
                puts("Unknown tray.");
            } else {
                /* Deliberate bug: the pointer is retained, enabling double-free. */
                free(slots[index]);
                in_use[index] = 0;
                puts("Tray returned.");
            }
        } else if (choice == 3) {
            int index = -1;
            char encoded[2 * CHUNK_SIZE + 2];
            unsigned char bytes[CHUNK_SIZE];
            printf("index> ");
            if (scanf("%d", &index) != 1) return 0;
            printf("data (hex)> ");
            if (scanf("%130s", encoded) != 1) return 0;
            discard_line();
            if (index < 0 || index >= SLOT_COUNT || slots[index] == NULL) {
                puts("Unknown tray.");
                continue;
            }
            int length = decode_hex(encoded, bytes, sizeof(bytes));
            if (length < 0) {
                puts("Invalid hex data.");
                continue;
            }
            memcpy(slots[index], bytes, (size_t)length);
            puts("Tray updated.");
        } else if (choice == 4) {
            puts("Serving the selected recipe...");
            if (callback_target.callback != NULL) {
                callback_target.callback();
            } else {
                puts("No serving callback is set.");
            }
        } else if (choice == 5) {
            puts("Closing the counter.");
            return 0;
        } else {
            puts("Unknown choice.");
        }
        fputs("choice> ", stdout);
    }
}
