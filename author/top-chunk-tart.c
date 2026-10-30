#define _GNU_SOURCE
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* A deliberately small teaching allocator.  Its top-size field is adjacent
 * to the arena so a one-slot overrun can corrupt the size accounting. */
struct arena_state {
    _Alignas(16) unsigned char bytes[0x100];
    size_t top_size;
    unsigned char *top;
    uintptr_t spare0;
    uintptr_t spare1;
    void (*serve)(void);
};

_Static_assert(offsetof(struct arena_state, top_size) == 0x100,
               "arena metadata layout changed");
_Static_assert(offsetof(struct arena_state, serve) == 0x120,
               "serving callback layout changed");

static struct arena_state arena;

struct allocation {
    unsigned char *pointer;
    size_t capacity;
};

static struct allocation slots[8];
static size_t slot_count;

static int read_line(char *buffer, size_t capacity) {
    size_t used = 0;
    while (used + 1 < capacity) {
        char byte;
        ssize_t count = read(STDIN_FILENO, &byte, 1);
        if (count < 0 && errno == EINTR)
            continue;
        if (count <= 0)
            return 0;
        if (byte == '\n')
            break;
        buffer[used++] = byte;
    }
    buffer[used] = '\0';
    return 1;
}

static int read_exact(void *destination, size_t length) {
    size_t used = 0;
    while (used < length) {
        ssize_t count = read(STDIN_FILENO, (unsigned char *)destination + used,
                             length - used);
        if (count < 0 && errno == EINTR)
            continue;
        if (count <= 0)
            return 0;
        used += (size_t)count;
    }
    char newline;
    while (read(STDIN_FILENO, &newline, 1) == 1 && newline != '\n') {}
    return 1;
}

static int read_number(const char *prompt, size_t *value) {
    char line[64];
    fputs(prompt, stdout);
    if (!read_line(line, sizeof line))
        return 0;
    char *end = NULL;
    errno = 0;
    unsigned long long parsed = strtoull(line, &end, 10);
    if (errno || end == line || *end != '\0')
        return -1;
    *value = (size_t)parsed;
    return 1;
}

static void ordinary_service(void) {
    puts("An ordinary serving. Nothing hidden.");
}

void reveal_recipe(void) {
    FILE *file = fopen("/flag", "r");
    char flag[128] = {0};
    if (!file) {
        puts("The secret recipe is unavailable.");
        return;
    }
    if (fgets(flag, sizeof flag, file)) {
        puts("The hidden tart recipe is:");
        puts(flag);
    }
    fclose(file);
}

static unsigned char *toy_malloc(size_t requested) {
    if (!requested || requested > 0x200)
        return NULL;
    size_t rounded = (requested + 0x0f) & ~(size_t)0x0f;
    if (rounded > arena.top_size)
        return NULL;
    unsigned char *result = arena.top;
    arena.top += rounded;
    arena.top_size -= rounded;
    return result;
}

static void print_menu(void) {
    puts("1) Reserve arena bytes");
    puts("2) Edit a recipe card");
    puts("3) Serve the special recipe");
    puts("4) Close the kitchen");
    fputs("> ", stdout);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    arena.top = arena.bytes;
    arena.top_size = sizeof arena.bytes;
    arena.serve = ordinary_service;

    puts("=== TOP-CHUNK TART ===");
    for (;;) {
        size_t choice = 0;
        print_menu();
        int status = read_number("", &choice);
        if (status == 0)
            return 0;
        if (status < 0) {
            puts("Invalid menu number.");
            continue;
        }
        if (choice == 1) {
            if (slot_count == sizeof slots / sizeof slots[0]) {
                puts("No more recipe cards fit.");
                continue;
            }
            size_t requested = 0;
            status = read_number("Size> ", &requested);
            if (status <= 0)
                return status == 0 ? 0 : 1;
            unsigned char *pointer = toy_malloc(requested);
            if (!pointer) {
                puts("The arena refuses that allocation.");
                continue;
            }
            slots[slot_count] = (struct allocation){pointer, requested};
            printf("Reserved slot %zu.\n", slot_count++);
        } else if (choice == 2) {
            size_t index = 0, length = 0;
            status = read_number("Slot> ", &index);
            if (status <= 0)
                return status == 0 ? 0 : 1;
            if (index >= slot_count) {
                puts("That recipe card does not exist.");
                continue;
            }
            status = read_number("Length> ", &length);
            if (status <= 0)
                return status == 0 ? 0 : 1;
            size_t limit = slots[index].capacity;
            if (index == 1)
                limit += sizeof(size_t); /* The sole bug: eight bytes too far. */
            if (!length || length > limit) {
                puts("That edit does not fit the card.");
                continue;
            }
            printf("Recipe bytes (%zu, then newline)> ", length);
            if (!read_exact(slots[index].pointer, length))
                return 0;
            puts("Recipe card updated.");
        } else if (choice == 3) {
            arena.serve();
        } else if (choice == 4) {
            puts("Service closed.");
            return 0;
        } else {
            puts("Choose 1, 2, 3, or 4.");
        }
    }
}
