#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* A genuine leaf routine: syscalls do not count as calls under the AMD64 ABI,
 * so its 128-byte red zone is used as the input buffer. The unchecked syscall
 * loop continues into the saved return address above it. */
__asm__(
    ".section .rodata\n"
    "red_zone_prompt:\n"
    ".asciz \"Recipe bytes> \"\n"
    ".text\n"
    ".global red_zone_reader\n"
    ".type red_zone_reader,@function\n"
    "red_zone_reader:\n"
    "    mov $1, %eax\n"
    "    mov $1, %edi\n"
    "    lea red_zone_prompt(%rip), %rsi\n"
    "    mov $14, %edx\n"
    "    syscall\n"
    "    lea -128(%rsp), %rsi\n"
    "    mov $144, %edx\n"
    ".Lread_more:\n"
    "    xor %eax, %eax\n"
    "    xor %edi, %edi\n"
    "    syscall\n"
    "    test %rax, %rax\n"
    "    jle .Linput_closed\n"
    "    add %rax, %rsi\n"
    "    sub %rax, %rdx\n"
    "    jne .Lread_more\n"
    "    ret\n"
    ".Linput_closed:\n"
    "    mov $60, %eax\n"
    "    xor %edi, %edi\n"
    "    syscall\n"
    "    ud2\n"
    ".size red_zone_reader, .-red_zone_reader\n"
    ".section .note.GNU-stack,\"\",@progbits\n");

extern void red_zone_reader(void);

void reveal_filling(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The zongzi filling is unavailable.");
        return;
    }
    ssize_t length = read(fd, flag, sizeof flag - 1);
    close(fd);
    if (length > 0) {
        puts("The hidden filling:");
        write(STDOUT_FILENO, flag, (size_t)length);
        write(STDOUT_FILENO, "\n", 1);
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== RED-ZONE ZONGZI ===");
    puts("A leaf routine takes a recipe in its red zone.");
    red_zone_reader();
    puts("Recipe accepted.");
    return 0;
}
