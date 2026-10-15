#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef FLAG_PATH
#define FLAG_PATH "/flag"
#endif

struct order_ticket {
    char recipe[32];
    void (*serve)(void);
    unsigned char spill_pad[256];
};

static struct order_ticket current;

_Static_assert(offsetof(struct order_ticket, serve) == 32,
               "the serving pointer must follow the 32-byte recipe");

__attribute__((noinline, used, noreturn)) static void reveal_recipe(void) {
    char flag[128];
    int fd = open(FLAG_PATH, O_RDONLY);
    if (fd < 0) _exit(1);
    ssize_t length = read(fd, flag, sizeof flag);
    if (length > 0) (void)write(STDOUT_FILENO, flag, (size_t)length);
    close(fd);
    _exit(0);
}

static void ordinary_service(void) {
    puts("通常のワッフルを配膳しました。");
}

static int read_full(void *buffer, size_t length) {
    size_t used = 0;
    while (used < length) {
        ssize_t got = read(STDIN_FILENO, (char *)buffer + used, length - used);
        if (got <= 0) return -1;
        used += (size_t)got;
    }
    return 0;
}

int main(void) {
    char line[32];
    setvbuf(stdout, NULL, _IONBF, 0);
    current.serve = ordinary_service;

    puts("=== Wraparound Waffle ===");
    puts("A length byte guards the recipe card; the full size controls the read.");
    fputs("recipe length> ", stdout);
    if (fgets(line, sizeof line, stdin) == NULL) return 0;

    errno = 0;
    char *end = NULL;
    unsigned long long requested = strtoull(line, &end, 10);
    if (errno != 0 || end == line || requested == 0 || requested > sizeof current) {
        puts("Invalid recipe length.");
        return 1;
    }

    /* Bug: validate a truncated byte, but use the original size below. */
    uint8_t checked_length = (uint8_t)requested;
    if (checked_length > sizeof current.recipe) {
        puts("Recipe card is too long.");
        return 1;
    }

    printf("Send %llu recipe bytes> ", requested);
    if (read_full(&current, (size_t)requested) < 0) return 0;
    puts("The recipe is ready.");
    current.serve();
    return 0;
}
