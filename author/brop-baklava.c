#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

__asm__(
    ".global tofu_brop_exit\n"
    ".type tofu_brop_exit, @function\n"
    "tofu_brop_exit:\n"
    "    mov $60, %eax\n"
    "    xor %edi, %edi\n"
    "    syscall\n"
);

__attribute__((noinline, used)) static void probe_stop(void) {
    puts("BROP probe survived.");
}

__attribute__((noinline, used)) static void secret_recipe(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The hidden baklava is unavailable.");
        return;
    }
    ssize_t length = read(fd, flag, sizeof(flag) - 1U);
    if (length > 0) {
        (void)write(STDOUT_FILENO, flag, (size_t)length);
    }
    close(fd);
}

static void take_order(void) {
    char recipe[48];
    puts("The baklava counter accepts a 48-byte recipe card.");
    puts("No error message will tell you whether the card returned safely.");
    (void)syscall(SYS_read, STDIN_FILENO, recipe, 512U);
    puts("Card accepted.");
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== BROP Baklava ==");
    puts("The counter is a fork-per-order service: crashes reveal only a closed connection.");
    take_order();
    return 0;
}
