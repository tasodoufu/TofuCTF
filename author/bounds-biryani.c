#define _GNU_SOURCE
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

struct recipe_index {
    const char *samples[2];
    void (*special)(void);
};

static void reveal_recipe(void)
{
    char flag[128];
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The secret pantry is closed.");
        return;
    }
    ssize_t n = read(fd, flag, sizeof(flag));
    close(fd);
    if (n > 0)
        (void)write(STDOUT_FILENO, flag, (size_t)n);
}

static struct recipe_index recipes = {
    .samples = {"saffron", "cardamom"},
    .special = reveal_recipe
};

__attribute__((noinline))
static void accept_order(void)
{
    char order[40];
    volatile size_t amount = 256;

    puts("Recipe> ");
    (void)read(STDIN_FILENO, order, amount);
    puts("Order received.");
}

int main(void)
{
    char slot_text[32];
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    printf("Sample slot (0-1)> ");
    if (!fgets(slot_text, sizeof(slot_text), stdin))
        return 0;
    unsigned long slot = strtoul(slot_text, NULL, 10);
    /* Intentional OOB read: slot 2 interprets the adjacent callback as a pointer. */
    printf("Recipe pointer: %p\n", (void *)recipes.samples[slot]);
    accept_order();
    return 0;
}
