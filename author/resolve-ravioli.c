#define _GNU_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

/* Deliberately explicit gadgets keep the ret2dlresolve exercise reproducible. */
__attribute__((naked, noinline, used)) void tofu_raw_read(void) {
    __asm__("xor %eax, %eax; syscall; ret");
}

__attribute__((naked, noinline, used)) void tofu_pop_rdi(void) {
    __asm__("pop %rdi; ret");
}

__attribute__((naked, noinline, used)) void tofu_pop_rsi(void) {
    __asm__("pop %rsi; ret");
}

__attribute__((naked, noinline, used)) void tofu_pop_rdx(void) {
    __asm__("pop %rdx; ret");
}

__attribute__((naked, noinline, used)) void tofu_pop_rbp(void) {
    __asm__("pop %rbp; ret");
}

__attribute__((naked, noinline, used)) void tofu_leave(void) {
    __asm__("leave; ret");
}

static char staging[4096];

/* Make the distant version-table lookup land in a mapped read-only page. */
__attribute__((section(".resolvepad"), used)) static const unsigned char resolve_padding[0x800] = {
    [0x44c] = 1,
};

static void take_order(void) {
    char order[64];
    puts("== Resolve Ravioli ==");
    puts("The ravioli counter accepts a 64-byte order card.");
    puts("The linker knows more recipes than this binary admits.");
    (void)syscall(SYS_read, STDIN_FILENO, order, 512U);
    puts("Order recorded.");
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    take_order();
    return 0;
}

/* Keep the writable staging area in the stripped artifact. */
int tofu_staging_size(void) {
    return (int)sizeof(staging);
}
