#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define SECRET_ORDER UINT64_C(0x41524550415f3754)

static void reveal_recipe(void) {
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
        static const char heading[] = "The seventh-argument recipe:\n";
        write(STDOUT_FILENO, heading, sizeof heading - 1);
        write(STDOUT_FILENO, flag, (size_t)length);
        write(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

/* AMD64 SysV passes integer arguments 1-6 in registers; argument 7 is at
 * [RSP+8] on function entry, immediately after the return address. */
__attribute__((noinline, used))
void seventh_arg_gate(uint64_t a1, uint64_t a2, uint64_t a3,
                      uint64_t a4, uint64_t a5, uint64_t a6,
                      uint64_t order_code) {
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    (void)a6;
    if (order_code == SECRET_ORDER)
        reveal_recipe();
    static const char rejected[] = "Wrong seventh order code.\n";
    write(STDOUT_FILENO, rejected, sizeof rejected - 1);
}

/* The vulnerable return lands at RSP % 16 == 0. This one-instruction shim
 * makes the gate enter with the normal SysV function-entry alignment. */
__attribute__((naked, used, noinline))
void ret_shim(void) {
    __asm__ volatile("ret\n");
}

__attribute__((noreturn, noinline, used))
void leave_kitchen(void) {
    _exit(0);
}

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
