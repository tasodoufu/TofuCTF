#define _GNU_SOURCE
#include <inttypes.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

jmp_buf jump_state;
int capture_checkpoint(void);

void reveal_recipe(void) {
    FILE *file = fopen("/flag", "r");
    char flag[128] = {0};
    if (!file) {
        puts("The secret recipe is unavailable.");
        return;
    }
    if (fgets(flag, sizeof flag, file)) {
        puts("The hidden lassi recipe is:");
        puts(flag);
    }
    fclose(file);
}

__asm__(
    ".text\n"
    ".globl capture_checkpoint\n"
    ".type capture_checkpoint,@function\n"
    "capture_checkpoint:\n"
    "subq $8, %rsp\n"
    "leaq jump_state(%rip), %rdi\n"
    "xorl %esi, %esi\n"
    "call _setjmp@PLT\n"
    ".globl checkpoint_resume\n"
    ".type checkpoint_resume,@function\n"
    "checkpoint_resume:\n"
    "addq $8, %rsp\n"
    "ret\n"
    ".size checkpoint_resume, .-checkpoint_resume\n"
    ".size capture_checkpoint, .-capture_checkpoint\n"
);

static int read_line(char *buffer, size_t capacity) {
    if (!fgets(buffer, (int)capacity, stdin))
        return 0;
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

static void menu(void) {
    char line[64];
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== LONGJMP LASSI ===");
    for (;;) {
        puts("1) Inspect the saved checkpoint");
        puts("2) Rewrite its saved destination");
        puts("3) Restore the checkpoint");
        puts("4) Close the counter");
        fputs("> ", stdout);
        if (!read_line(line, sizeof line))
            return;
        switch (atoi(line)) {
        case 1:
            printf("Mangled saved RIP: %016" PRIx64 "\n",
                   (uint64_t)jump_state[0].__jmpbuf[7]);
            break;
        case 2: {
            fputs("New mangled RIP (hex)> ", stdout);
            if (!read_line(line, sizeof line))
                return;
            char *end = NULL;
            uint64_t value = strtoull(line, &end, 16);
            if (end == line || *end != '\0') {
                puts("That is not a hexadecimal address.");
                break;
            }
            jump_state[0].__jmpbuf[7] = (long int)value;
            puts("Checkpoint destination updated.");
            break;
        }
        case 3:
            puts("Restoring the kitchen checkpoint...");
            longjmp(jump_state, 1);
        case 4:
            puts("Service closed.");
            return;
        default:
            puts("Choose 1, 2, 3, or 4.");
            break;
        }
    }
}

int main(void) {
    if (capture_checkpoint() == 0) {
        menu();
        return 0;
    }
    puts("The kitchen checkpoint was restored.");
    return 0;
}
