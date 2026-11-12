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
        static const char heading[] = "The pavlova recipe:\n";
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

__attribute__((noinline, used))
int read_exact(int fd, void *buffer, size_t length) {
    size_t used = 0;
    while (used < length) {
        ssize_t count = read(fd, (unsigned char *)buffer + used, length - used);
        if (count <= 0)
            return -1;
        used += (size_t)count;
    }
    return 0;
}

extern void take_tasting_card(void);

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== Overread Pavlova ===");
    puts("The tasting note is short, but the printer reads one line too far.");
    take_tasting_card();
    __asm__ volatile(".global after_tasting_card\nafter_tasting_card:");
    puts("The ordinary pavlova is served.");
    return 0;
}

/* The first 64 bytes are filled exactly. The 80-byte receipt print includes
 * saved RBP and the PIE return address; the second 88-byte read overruns the
 * 64-byte card and reaches saved RIP. These fixed frames make the lesson stable. */
__asm__(
    ".section .rodata\n"
    "pavlova_first_prompt:\n"
    ".asciz \"Tasting note (64 bytes)> \"\n"
    "pavlova_second_prompt:\n"
    ".asciz \"Recipe card (88 bytes)> \"\n"
    ".text\n"
    ".global take_tasting_card\n"
    ".type take_tasting_card,@function\n"
    "take_tasting_card:\n"
    "    push %rbp\n"
    "    mov %rsp,%rbp\n"
    "    sub $0x40,%rsp\n"
    "    mov $1,%edi\n"
    "    lea pavlova_first_prompt(%rip),%rsi\n"
    "    mov $25,%edx\n"
    "    call write@PLT\n"
    "    xor %edi,%edi\n"
    "    lea -0x40(%rbp),%rsi\n"
    "    mov $0x40,%edx\n"
    "    call read_exact@PLT\n"
    "    test %eax,%eax\n"
    "jne .Linput_error\n"
    "    mov $1,%edi\n"
    "    lea -0x40(%rbp),%rsi\n"
    "    mov $0x50,%edx\n"
    "    call write@PLT\n"
    "    mov $1,%edi\n"
    "    lea pavlova_second_prompt(%rip),%rsi\n"
    "    mov $24,%edx\n"
    "    call write@PLT\n"
    "    xor %edi,%edi\n"
    "    lea -0x40(%rbp),%rsi\n"
    "    mov $0x58,%edx\n"
    "    call read_exact@PLT\n"
    "    test %eax,%eax\n"
    "jne .Linput_error\n"
    "    leave\n"
    "    ret\n"
    ".Linput_error:\n"
    "    mov $1,%edi\n"
    "    call _exit@PLT\n"
    "    ud2\n"
    ".size take_tasting_card,.-take_tasting_card\n");
