#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

struct action_view {
    void (*callback)(void);
    char label[24];
};

union payload {
    char note[32];
    struct action_view action;
};

struct order {
    uint32_t kind;
    char name[16];
    union payload payload;
};

static struct order current;

enum { KIND_NOTE = 1, KIND_ACTION = 2 };

__attribute__((noinline, used, noreturn)) static void reveal_recipe(void) {
    char flag[128];
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) _exit(1);
    ssize_t length = read(fd, flag, sizeof flag);
    if (length > 0) (void)write(STDOUT_FILENO, flag, (size_t)length);
    close(fd);
    _exit(0);
}

static void ordinary_service(void) {
    puts("通常の注文を配膳しました。");
}

static void discard_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
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

static void menu(void) {
    puts("=== Type-Confused Cannoli ===");
    puts("1) set order kind");
    puts("2) edit note");
    puts("3) serve");
    puts("4) quit");
    fputs("> ", stdout);
}

int main(void) {
    char line[32];
    memset(&current, 0, sizeof current);
    current.kind = KIND_NOTE;
    current.payload.action.callback = ordinary_service;
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    for (;;) {
        menu();
        if (!fgets(line, sizeof line, stdin)) return 0;
        switch (strtol(line, NULL, 10)) {
        case 1:
            puts("1) note  2) action");
            fputs("kind> ", stdout);
            if (!fgets(line, sizeof line, stdin)) return 0;
            current.kind = (uint32_t)strtoul(line, NULL, 10);
            puts("注文種別を更新しました。");
            break;
        case 2:
            fputs("note (32 bytes)> ", stdout);
            /* The note editor ignores the active kind and aliases the action object. */
            if (read_full(current.payload.note, sizeof current.payload.note) < 0) return 0;
            puts("メモを更新しました。");
            break;
        case 3:
            if (current.kind == KIND_ACTION) {
                current.payload.action.callback();
            } else {
                puts("メモ注文: ");
                puts(current.payload.note);
            }
            break;
        case 4:
            return 0;
        default:
            puts("その番号はありません。");
            discard_line();
            break;
        }
    }
}
