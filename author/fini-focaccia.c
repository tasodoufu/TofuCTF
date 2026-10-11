#define _GNU_SOURCE

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void (*recipe_board[4])(void);

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

static void print_menu(void) {
    puts("== Fini Focaccia ==");
    puts("The focaccia counter keeps its final recipe in a hidden exit tray.");
    puts("A signed tray number is accepted, but only the upper edge is checked.");
    printf("Recipe board: %p\n", (void *)recipe_board);
    puts("Choose a recipe slot and write its serving address.");
}

int main(void) {
    long slot = 0;
    unsigned long value = 0;

    setvbuf(stdout, NULL, _IONBF, 0);
    print_menu();
    fputs("slot> ", stdout);
    if (scanf("%ld", &slot) != 1) {
        puts("Invalid slot.");
        return 1;
    }
    if (slot >= (long)(sizeof(recipe_board) / sizeof(recipe_board[0]))) {
        puts("That tray is outside the counter.");
        return 1;
    }
    fputs("address> ", stdout);
    if (scanf("%lx", &value) != 1) {
        puts("Invalid address.");
        return 1;
    }

    /* The missing lower-bound check turns this into a write before the board. */
    recipe_board[slot] = (void (*)(void))(uintptr_t)value;
    puts("The final tray is updated. Closing the counter...");
    return 0;
}
