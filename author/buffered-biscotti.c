#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

__attribute__((noreturn, noinline, used))
void serve_recipe(void) {
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
        static const char heading[] = "The biscotti recipe:\n";
        write(STDOUT_FILENO, heading, sizeof heading - 1);
        write(STDOUT_FILENO, flag, (size_t)length);
        write(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

__attribute__((naked, noinline, used))
void ret_shim(void) {
    __asm__ volatile("ret\n");
}

/* The 64-byte raw-read card is intentionally stack-based. The saved RIP is
 * 72 bytes from its start; this fixed assembly keeps the author proof stable. */
__asm__(
    ".section .rodata\n"
    "card_prompt:\n"
    ".asciz \"Raw recipe card> \"\n"
    ".text\n"
    ".global take_raw_card\n"
    ".type take_raw_card,@function\n"
    "take_raw_card:\n"
    "    push %rbp\n"
    "    mov %rsp,%rbp\n"
    "    sub $0x40,%rsp\n"
    "    mov $1,%edi\n"
    "    lea card_prompt(%rip),%rsi\n"
    "    mov $17,%edx\n"
    "    call write@PLT\n"
    "    xor %edi,%edi\n"
    "    lea -0x40(%rbp),%rsi\n"
    "    mov $0xc0,%edx\n"
    "    call read@PLT\n"
    "    leave\n"
    "    ret\n"
    ".size take_raw_card,.-take_raw_card\n");

extern void take_raw_card(void);

int main(void) {
    char name[32];
    static const char name_prompt[] = "Name> ";
    write(STDOUT_FILENO, name_prompt, sizeof name_prompt - 1);
    if (!fgets(name, sizeof name, stdin))
        return 0;
    take_raw_card();
    return 0;
}
