#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct recipe_tray {
    char label[16];
    char note[32];
    void (*handler)(void);
};

static struct recipe_tray tray;

static void inspect_tray(void) {
    puts("The tiramisu recipe is folded into a neat little tray.");
}

__attribute__((used, noinline)) static void secret_tiramisu(void) {
    FILE *fp = fopen("/flag", "r");
    char flag[128] = {0};
    if (!fp) {
        puts("The dessert book is missing.");
        _exit(1);
    }
    if (fgets(flag, sizeof flag, fp))
        printf("A secret tiramisu recipe appears: %s", flag);
    fclose(fp);
    _exit(0);
}

static int read_full(void *destination, size_t size) {
    unsigned char *cursor = destination;
    size_t received = 0;
    while (received < size) {
        ssize_t count = read(STDIN_FILENO, cursor + received, size - received);
        if (count <= 0)
            return 0;
        received += (size_t)count;
    }
    return 1;
}

static void menu(void) {
    puts("1) write tiramisu recipe");
    puts("2) taste tiramisu");
    puts("3) leave kitchen");
    fputs("> ", stdout);
}

static void write_recipe(void) {
    char line[32];
    unsigned long requested;
    uint8_t narrowed;

    fputs("Requested recipe bytes: ", stdout);
    if (!fgets(line, sizeof line, stdin))
        return;
    requested = strtoul(line, NULL, 10);
    narrowed = (uint8_t)requested;
    printf("Recipe bytes (stored as %u): ", narrowed);
    if (!read_full(tray.note, narrowed)) {
        puts("The recipe was cut short.");
        return;
    }
    puts("\nRecipe stored.");
}

int main(void) {
    tray.handler = inspect_tray;
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== TRUNCATION TIRAMISU ===");
    puts("The dessert ledger narrows a large recipe measurement to one byte.");

    for (;;) {
        menu();
        char line[16];
        if (!fgets(line, sizeof line, stdin))
            return 0;
        switch (strtol(line, NULL, 10)) {
        case 1:
            write_recipe();
            break;
        case 2:
            tray.handler();
            break;
        case 3:
            return 0;
        default:
            puts("That is not on the menu.");
            break;
        }
    }
}
