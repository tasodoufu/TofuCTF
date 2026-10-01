#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

/* Deliberate ret2csu-style gadgets for learning multi-register setup. */
__asm__(
    ".global tofu_csu_pop\n"
    ".type tofu_csu_pop, @function\n"
    "tofu_csu_pop:\n"
    "    pop %rbx\n"
    "    pop %rbp\n"
    "    pop %r12\n"
    "    pop %r13\n"
    "    pop %r14\n"
    "    pop %r15\n"
    "    ret\n"
    ".global tofu_csu_call\n"
    ".type tofu_csu_call, @function\n"
    "tofu_csu_call:\n"
    "    mov %r15, %rdx\n"
    "    mov %r14, %rsi\n"
    "    mov %r13, %rdi\n"
    "    call *(%r12,%rbx,8)\n"
    "    add $1, %rbx\n"
    "    cmp %rbp, %rbx\n"
    "    jne tofu_csu_call\n"
    "    add $8, %rsp\n"
    "    pop %rbx\n"
    "    pop %rbp\n"
    "    pop %r12\n"
    "    pop %r13\n"
    "    pop %r14\n"
    "    pop %r15\n"
    "    ret\n"
);

static const char required_ingredient[] = "cinnamon";

static void unlock(uint64_t order_code, const char *ingredient, size_t portions) {
    char flag[128];
    int fd;
    ssize_t length;

    if (order_code != UINT64_C(0x1337133713371337) ||
        portions != 7 || strcmp(ingredient, required_ingredient) != 0) {
        puts("The churro order is missing a valid seal.");
        return;
    }

    fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The secret recipe is unavailable.");
        return;
    }
    length = read(fd, flag, sizeof(flag));
    if (length > 0) write(STDOUT_FILENO, flag, (size_t)length);
    close(fd);
}

/* A writable dispatch table gives the indirect call a stable target. */
static void (*dispatch_target)(uint64_t, const char *, size_t)
    __attribute__((used)) = unlock;

static void take_order(void) {
    char order[64];

    puts("The churro counter accepts a 64-byte order card.");
    puts("Write the order and finish with a newline:");
    (void)syscall(SYS_read, STDIN_FILENO, order, 400U);
    puts("Order filed.");
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== CSU Churro ==");
    puts("The kitchen has a register-loading shortcut, but the order card is too small.");
    take_order();
    return 0;
}
