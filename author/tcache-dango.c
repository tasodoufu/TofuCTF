#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct dango {
    char label[32];
    void (*serve)(void);
};

static struct dango *tray;
static _Alignas(16) unsigned char service_area[64];

static void ordinary_dango(void) {
    puts("The dango is served with an ordinary sauce.");
}

__attribute__((used, noinline)) static void secret_dango(void) {
    FILE *fp = fopen("/flag", "r");
    char flag[128] = {0};
    if (!fp) {
        puts("The syrup kettle is empty.");
        exit(1);
    }
    if (fgets(flag, sizeof flag, fp)) {
        puts("A hidden dango recipe appears:");
        puts(flag);
    }
    fclose(fp);
    exit(0);
}

static int read_full(void *destination, size_t count) {
    unsigned char *cursor = destination;
    size_t received = 0;
    while (received < count) {
        ssize_t result = read(STDIN_FILENO, cursor + received, count - received);
        if (result <= 0)
            return -1;
        received += (size_t)result;
    }
    return 0;
}

static void menu(void) {
    puts("1) add dango");
    puts("2) remove dango");
    puts("3) label dango");
    puts("4) serve dango");
    puts("5) exit");
    fputs("> ", stdout);
}

int main(void) {
    char line[16];
    *(void (**)(void))service_area = ordinary_dango;
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== TCACHE DANGO ===");
    puts("The shop reuses freed trays to save syrup.");
    printf("Serving desk: %p\n", (void *)service_area);

    for (;;) {
        menu();
        if (!fgets(line, sizeof line, stdin))
            return 0;
        switch (atoi(line)) {
        case 1:
            tray = malloc(sizeof *tray);
            if (!tray) {
                puts("No tray is available.");
                return 1;
            }
            tray->serve = ordinary_dango;
            printf("Dango tray: %p\n", (void *)tray);
            break;
        case 2:
            if (!tray) {
                puts("There is no dango tray.");
                break;
            }
            free(tray);
            /* The old order system forgets to clear the tray pointer. */
            puts("The dango tray was returned.");
            break;
        case 3:
            if (!tray) {
                puts("There is no dango tray.");
                break;
            }
            puts("Write a 32-byte label:");
            if (read_full(tray->label, sizeof tray->label) < 0)
                return 1;
            puts("Label recorded.");
            break;
        case 4:
            (*(void (**)(void))service_area)();
            break;
        case 5:
            return 0;
        default:
            puts("That is not on the menu.");
            break;
        }
    }
}
