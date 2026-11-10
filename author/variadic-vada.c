#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

/* The fixed first parameter is intentionally ignored.  Under the SysV AMD64
 * variadic-call ABI, RSI carries the first integer ellipsis argument and AL
 * carries the number of vector registers used for variadic floating arguments.
 * This gate requires zero such arguments, so a ROP caller must clear EAX. */
__attribute__((noreturn, noinline, used))
void reveal_recipe(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        static const char unavailable[] = "The secret recipe is unavailable.\n";
        write(STDOUT_FILENO, unavailable, sizeof unavailable - 1);
        _exit(1);
    }
    ssize_t length = read(fd, flag, sizeof flag - 1);
    close(fd);
    if (length > 0) {
        static const char heading[] = "The variadic recipe:\n";
        write(STDOUT_FILENO, heading, sizeof heading - 1);
        write(STDOUT_FILENO, flag, (size_t)length);
        write(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

/* This declaration documents the real variadic prototype; file-scope assembly
 * supplies the body so no compiler-generated prologue obscures the AL check. */
extern void variadic_gate(uint64_t ignored, ...);
__asm__(
    ".text\n"
    ".global variadic_gate\n"
    ".type variadic_gate,@function\n"
    "variadic_gate:\n"
    "    testb %al, %al\n"
    "    jne 1f\n"
    "    cmpl $0x56414441, %esi\n" /* integer ellipsis argument: \"VADA\" */
    "    jne 1f\n"
    /* Entry via the vulnerable RET has RSP % 16 == 0; CALL preserves ABI. */
    "    call reveal_recipe\n"
    "    ud2\n"
    "1:\n"
    "    mov $1, %edi\n"
    "    call _exit@PLT\n"
    "    ud2\n"
    ".size variadic_gate,.-variadic_gate\n");

/* Deliberate author-visible ROP gadgets. */
__attribute__((naked, noinline, used))
void pop_rsi_ret(void) {
    __asm__ volatile("pop %rsi\nret\n");
}

__attribute__((naked, noinline, used))
void pop_rax_ret(void) {
    __asm__ volatile("pop %rax\nret\n");
}

/* Stable 64-byte local buffer and saved-RIP offset (72 bytes). */
__asm__(
    ".section .rodata\n"
    "order_prompt:\n"
    ".asciz \"Recipe card> \"\n"
    ".text\n"
    ".global take_order\n"
    ".type take_order,@function\n"
    "take_order:\n"
    "    push %rbp\n"
    "    mov %rsp,%rbp\n"
    "    sub $0x40,%rsp\n"
    "    mov $1,%edi\n"
    "    lea order_prompt(%rip),%rsi\n"
    "    mov $13,%edx\n"
    "    call write@PLT\n"
    "    xor %edi,%edi\n"
    "    lea -0x40(%rbp),%rsi\n"
    "    mov $0xc0,%edx\n"
    "    call read@PLT\n"
    "    leave\n"
    "    ret\n"
    ".size take_order,.-take_order\n");

extern void take_order(void);

int main(void) {
    take_order();
    return 0;
}
