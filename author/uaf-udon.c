#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef void (*serve_fn)(const char *);
struct order { char name[64]; serve_fn serve; };
static struct order *dangling;

static void set_io(void) { setvbuf(stdin, NULL, _IONBF, 0); setvbuf(stdout, NULL, _IONBF, 0); }
static void serve_plain(const char *name) { printf("Udon order: %s\n", name); }
__attribute__((used)) static void secret_udon(const char *unused) {
    FILE *fp; char flag[128] = {0}; (void)unused;
    fp = fopen("/flag", "r");
    if (!fp) { puts("The kitchen is closed."); exit(1); }
    fgets(flag, sizeof(flag), fp); fclose(fp);
    puts("A hidden udon recipe appears:"); puts(flag); exit(0);
}
static void menu(void) { puts("1) order  2) cancel  3) rename  4) serve  5) exit"); printf("> "); }
int main(void) {
    char buf[96]; int choice; set_io();
    puts("=== UAF UDON ===");
    for (;;) {
        menu(); if (!fgets(buf, sizeof(buf), stdin)) return 0; choice = atoi(buf);
        if (choice == 1) {
            dangling = malloc(sizeof(*dangling));
            dangling->serve = serve_plain;
            printf("Name: "); fgets(dangling->name, sizeof(dangling->name), stdin);
            puts("Order accepted.");
        } else if (choice == 2) {
            if (!dangling) { puts("No order."); continue; }
            free(dangling); puts("Order cancelled.");
        } else if (choice == 3) {
            if (!dangling) { puts("No order."); continue; }
            printf("New name: ");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
            read(0, dangling->name, sizeof(dangling->name) + sizeof(dangling->serve));
#pragma GCC diagnostic pop
            puts("Name updated.");
        } else if (choice == 4) {
            if (!dangling) { puts("No order."); continue; }
            dangling->serve(dangling->name);
        } else if (choice == 5) return 0;
    }
}
