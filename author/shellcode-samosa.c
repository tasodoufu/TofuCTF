#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void menu(void) {
    puts("1) pack shellcode");
    puts("2) serve shellcode");
    puts("3) leave kitchen");
    fputs("> ", stdout);
}

int main(void) {
    char kitchen[512];
    char line[16];
    ssize_t packed = 0;

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== SHELLCODE SAMOSA ===");
    puts("The kitchen accepts one executable recipe. Can you make it read the hidden menu?");

    for (;;) {
        menu();
        if (!fgets(line, sizeof line, stdin))
            return 0;
        switch (atoi(line)) {
        case 1:
            puts("Send your recipe bytes now.");
            packed = read(STDIN_FILENO, kitchen, sizeof kitchen);
            if (packed <= 0) {
                puts("The recipe was empty.");
                return 1;
            }
            printf("Packed %zd bytes.\n", packed);
            break;
        case 2:
            if (packed <= 0) {
                puts("There is no recipe to serve.");
                break;
            }
            puts("Serving the executable recipe...");
            void *entry = kitchen;
            void (*execute)(void);
            memcpy(&execute, &entry, sizeof execute);
            execute();
            puts("The recipe returned safely.");
            break;
        case 3:
            return 0;
        default:
            puts("That is not on today's menu.");
            break;
        }
    }
}
