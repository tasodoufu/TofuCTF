#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

/* The syscall gadgets are deliberately present for the ret2syscall lesson. */
__asm__(
    ".global tofu_pop_rax\n"
    "tofu_pop_rax:\n"
    "    pop %rax\n"
    "    ret\n"
    ".global tofu_pop_rdi\n"
    "tofu_pop_rdi:\n"
    "    pop %rdi\n"
    "    ret\n"
    ".global tofu_pop_rsi\n"
    "tofu_pop_rsi:\n"
    "    pop %rsi\n"
    "    ret\n"
    ".global tofu_pop_rdx\n"
    "tofu_pop_rdx:\n"
    "    pop %rdx\n"
    "    ret\n"
    ".global tofu_xchg_rax_rdi\n"
    "tofu_xchg_rax_rdi:\n"
    "    xchg %rax, %rdi\n"
    "    ret\n"
    ".global tofu_syscall_ret\n"
    "tofu_syscall_ret:\n"
    "    syscall\n"
    "    ret\n"
);

static unsigned char scratch[0x200] __attribute__((used));

static void take_recipe(void) {
    char buffer[64];

    puts("The sundae counter accepts a 64-byte recipe card.");
    puts("Write the complete order and finish with a newline:");
    /* syscall avoids a compiler-side bounds check: the overflow is the lesson. */
    (void)syscall(SYS_read, STDIN_FILENO, buffer, 512U);
    puts("Order received.");
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== Syscall Sundae ==");
    puts("The cashier checks the dessert, but not the size of the recipe card.");
    take_recipe();
    return 0;
}
