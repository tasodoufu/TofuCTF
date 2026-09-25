#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* The table is deliberately placed in writable data for this lesson. */
typedef void (*action_t)(void);
action_t action_table[4];

static void menu(void) {
    puts("1) inspect gnocchi");
    puts("2) edit recipe");
    puts("3) serve gnocchi");
    puts("4) close kitchen");
    fputs("> ", stdout);
}

static int read_line(char *buffer, size_t size) {
    if (!fgets(buffer, size, stdin))
        return 0;
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

static void inspect_gnocchi(void) {
    puts("The gnocchi are arranged in four recipe slots.");
}

static void serve_gnocchi(void) {
    puts("Gnocchi served with a quiet garlic sauce.");
}

__attribute__((used, noinline)) static void secret_gnocchi(void) {
    FILE *fp = fopen("/flag", "r");
    char flag[128] = {0};
    if (!fp) {
        puts("The recipe book is missing.");
        _exit(1);
    }
    if (fgets(flag, sizeof flag, fp))
        printf("A hidden gnocchi recipe appears: %s", flag);
    fclose(fp);
    _exit(0);
}

static void edit_recipe(void) {
    char line[64];
    char *end;
    long index;
    unsigned long long value;

    fputs("Recipe slot: ", stdout);
    if (!read_line(line, sizeof line))
        return;
    index = strtol(line, &end, 10);
    if (end == line || *end != '\0' || index > 3) {
        puts("That slot is not on the menu.");
        return;
    }

    fputs("New handler address (hex): ", stdout);
    if (!read_line(line, sizeof line))
        return;
    value = strtoull(line, &end, 16);
    if (end == line || *end != '\0') {
        puts("That is not a recipe address.");
        return;
    }

    /* Negative indices are accepted, allowing a write before the table. */
    action_table[index] = (action_t)value;
    puts("Recipe slot updated.");
}

static void close_kitchen(void) {
    exit(0);
}

int main(void) {
    char line[32];
    action_table[0] = inspect_gnocchi;
    action_table[1] = edit_recipe;
    action_table[2] = serve_gnocchi;
    action_table[3] = close_kitchen;

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== GOT GNOCCHI ===");
    puts("The recipe ledger trusts every slot number it receives.");

    for (;;) {
        menu();
        if (!read_line(line, sizeof line))
            return 0;
        long choice = strtol(line, NULL, 10);
        if (choice < 1 || choice > 4) {
            puts("That is not on the menu.");
            continue;
        }
        action_table[choice - 1]();
    }
}
