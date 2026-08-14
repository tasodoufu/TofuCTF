#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef void (*serve_fn)(const char *);
struct tray { char label[64]; serve_fn serve; };
static struct tray *current;

static void io(void) { setvbuf(stdin, NULL, _IONBF, 0); setvbuf(stdout, NULL, _IONBF, 0); }
static void serve_plain(const char *label) { printf("Sushi tray: %s\n", label); }
__attribute__((used)) static void secret_sushi(const char *unused) {
    FILE *fp; char flag[128] = {0}; (void)unused;
    fp = fopen("/flag", "r");
    if (!fp) { puts("The sushi bar is closed."); exit(1); }
    fgets(flag, sizeof(flag), fp); fclose(fp);
    puts("A secret sushi recipe appears:"); puts(flag); exit(0);
}
static void menu(void) { puts("1) prepare  2) discard  3) label  4) serve  5) exit"); printf("> "); }
int main(void) {
    char buf[32]; int choice; io();
    puts("=== SIGNED SUSHI ===");
    current = malloc(sizeof(*current));
    current->serve = serve_plain;
    for (;;) {
        menu(); if (!fgets(buf, sizeof(buf), stdin)) return 0; choice = atoi(buf);
        if (choice == 1) {
            printf("Label: "); fgets(current->label, sizeof(current->label), stdin);
            puts("Tray prepared.");
        } else if (choice == 2) {
            printf("Label bytes (-1..64): ");
            if (!fgets(buf, sizeof(buf), stdin)) return 0;
            int bytes = atoi(buf);
            if (bytes > 64) { puts("Too many bytes."); continue; }
            printf("New label: ");
            read(0, current->label, (size_t)bytes);
            puts("Tray relabelled.");
        } else if (choice == 3) {
            printf("Label: "); fgets(current->label, sizeof(current->label), stdin);
            puts("Tray relabelled.");
        } else if (choice == 4) {
            current->serve(current->label);
        } else if (choice == 5) return 0;
    }
}
