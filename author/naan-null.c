#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

/* The two callbacks intentionally live in the same 0x100-byte code window.
 * A one-byte overwrite of the callback pointer is enough to redirect it. */
__attribute__((aligned(256), noinline, used)) static void serve_order(void) {
    puts("The naan is served. Nothing unusual was found in the order.");
}

__attribute__((noinline, used)) static void secret_recipe(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The hidden naan is unavailable.");
        return;
    }
    ssize_t length = read(fd, flag, sizeof(flag) - 1U);
    if (length > 0) {
        (void)write(STDOUT_FILENO, flag, (size_t)length);
    }
    close(fd);
}

struct order_card {
    char topping[32];
    void (*finish)(void);
};

static void take_order(void) {
    struct order_card card = {.finish = serve_order};
    puts("== Naan Null ==");
    puts("The naan counter accepts a 32-byte topping card.");
    puts("A single extra byte may change where the order is finished.");
    (void)syscall(SYS_read, STDIN_FILENO, card.topping, sizeof(card.topping) + 1U);
    card.finish();
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    take_order();
    return 0;
}
