#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct order {
    char note[32];
    unsigned char seal;
};

static struct order current;
static void (*serve)(void);

static void ordinary(void) {
    puts("The onigiri is served without a special seal.");
}

static void secret_onigiri(void) {
    FILE *fp = fopen("/flag", "r");
    char flag[128] = {0};
    if (!fp) {
        puts("The rice cooker is closed.");
        exit(1);
    }
    if (fgets(flag, sizeof flag, fp)) {
        puts("A hidden onigiri recipe appears:");
        puts(flag);
    }
    fclose(fp);
    exit(0);
}

static ssize_t legacy_read(void *destination, size_t count) {
    return read(STDIN_FILENO, destination, count);
}

static void menu(void) {
    puts("1) write order");
    puts("2) seal order");
    puts("3) serve order");
    puts("4) exit");
    fputs("> ", stdout);
}

int main(void) {
    char line[16];
    serve = ordinary;
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== OFF-BY-ONE ONIGIRI ===");
    for (;;) {
        menu();
        if (!fgets(line, sizeof line, stdin))
            return 0;
        switch (atoi(line)) {
        case 1:
            puts("Write exactly one order note.");
            /* The note is 32 bytes, but this legacy counter accepts 33. */
            if (legacy_read(current.note, sizeof current.note + 1) < 0)
                return 0;
            puts("Order recorded.");
            break;
        case 2:
            if (current.seal == 0x42) {
                serve = secret_onigiri;
                puts("The hidden seal is accepted.");
            } else {
                puts("That seal is not valid.");
            }
            break;
        case 3:
            serve();
            break;
        case 4:
            return 0;
        default:
            puts("That order is not on the menu.");
            break;
        }
    }
}
