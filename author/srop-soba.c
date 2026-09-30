#define _DEFAULT_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

/* Deliberate gadgets for learning sigreturn-oriented programming. */
__asm__(
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
    ".global tofu_pop_rax\n"
    "tofu_pop_rax:\n"
    "    pop %rax\n"
    "    ret\n"
    ".global tofu_set_rax_15\n"
    "tofu_set_rax_15:\n"
    "    mov $15, %eax\n"
    "    ret\n"
    ".global tofu_syscall_ret\n"
    "tofu_syscall_ret:\n"
    "    syscall\n"
    "    ret\n"
);

static void serve_soba(void) {
    char recipe[64];

    printf("The soba tray is at %p.\n", (void *)recipe);
    puts("The kitchen accepts a recipe card, but never checks its length.");
    puts("Send the recipe now:");
    (void)syscall(SYS_read, STDIN_FILENO, recipe, 1024U);
    puts("Recipe filed.");
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== SROP Soba ==");
    puts("The noodle counter forgot that a saved CPU state is also data.");
    serve_soba();
    return 0;
}
