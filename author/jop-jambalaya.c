#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

/* These deliberately small gadgets form a jump-oriented dispatch chain. */
__asm__(
    ".global tofu_jop_entry\n"
    ".type tofu_jop_entry, @function\n"
    "tofu_jop_entry:\n"
    "    pop %r12\n"
    "    pop %r13\n"
    "    pop %r14\n"
    "    jmp *%r13\n"
    ".global tofu_jop_call\n"
    ".type tofu_jop_call, @function\n"
    "tofu_jop_call:\n"
    "    call *%r12\n"
    "    jmp *%r14\n"
    ".global tofu_jop_finish\n"
    ".type tofu_jop_finish, @function\n"
    "tofu_jop_finish:\n"
    "    mov $60, %eax\n"
    "    xor %edi, %edi\n"
    "    syscall\n"
);

static void ordinary_recipe(void) {
    puts("The ordinary jambalaya is served.");
}

__attribute__((noinline, used)) static void secret_recipe(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The secret recipe is unavailable.");
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
    puts("The jambalaya counter accepts a 48-byte recipe card.");
    puts("The card reader follows the return address without checking the card size:");
    (void)syscall(SYS_read, STDIN_FILENO, recipe, 512U);
    puts("Recipe filed.");
    ordinary_recipe();
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== JOP Jambalaya ==");
    puts("A jump-oriented kitchen routes each step through an indirect dispatcher.");
    take_order();
    return 0;
}
