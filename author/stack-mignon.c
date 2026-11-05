#define _GNU_SOURCE
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/* The dedicated ROP target checks its entry stack alignment.  A ret shim is
 * required because the vulnerable function's saved RIP is reached with RSP
 * 16-byte aligned, while a normally-called AMD64 function enters at RSP % 16=8. */
__attribute__((naked, used, noinline, noreturn))
void alignment_gate(void) {
    __asm__ volatile(
        "testb $8, %spl\n"
        "jz 1f\n"
        "sub $8, %rsp\n"
        "call reveal_recipe\n"
        "ud2\n"
        "1:\n"
        "xor %edi, %edi\n"
        "call _exit@PLT\n"
        "ud2\n");
}

__attribute__((naked, used, noinline))
void alignment_ret(void) {
    __asm__ volatile("ret\n");
}

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
        static const char heading[] = "The secret recipe:\n";
        write(STDOUT_FILENO, heading, sizeof heading - 1);
        write(STDOUT_FILENO, flag, (size_t)length);
        write(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

/* Keep the vulnerable frame explicit so the lesson's 72-byte offset is stable. */
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
