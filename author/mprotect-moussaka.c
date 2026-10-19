#define _GNU_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

/* A page-aligned staging area holds attacker-supplied machine code. */
__attribute__((used, aligned(4096))) static char recipe_stage[4096];

__attribute__((naked, used)) void gadget_pop_rax(void) {
    __asm__ volatile("pop %rax; ret");
}

__attribute__((naked, used)) void gadget_pop_rdi(void) {
    __asm__ volatile("pop %rdi; ret");
}

__attribute__((naked, used)) void gadget_pop_rsi(void) {
    __asm__ volatile("pop %rsi; ret");
}

__attribute__((naked, used)) void gadget_pop_rdx(void) {
    __asm__ volatile("pop %rdx; ret");
}

__attribute__((naked, used)) void gadget_syscall(void) {
    __asm__ volatile("syscall; ret");
}

__attribute__((noinline)) static void take_order(void) {
    char order_card[64];

    puts("Send machine-code recipe (up to 256 bytes):");
    (void)syscall(SYS_read, STDIN_FILENO, recipe_stage, 256);
    puts("Send the order card:");
    /* The kitchen trusts the card length and overruns its 64-byte tray. */
    (void)syscall(SYS_read, STDIN_FILENO, order_card, 512);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== Mprotect Moussaka ==");
    puts("The kitchen keeps recipe code on a non-executable page.");
    take_order();
    return 0;
}
