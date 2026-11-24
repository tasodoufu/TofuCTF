#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

static void write_all(int fd, const void *buffer, size_t length) {
    const unsigned char *cursor = buffer;
    while (length > 0) {
        ssize_t result = write(fd, cursor, length);
        if (result <= 0)
            _exit(1);
        cursor += (size_t)result;
        length -= (size_t)result;
    }
}

__attribute__((noreturn, noinline, used))
void reveal_recipe(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        static const char unavailable[] = "The recipe is unavailable.\n";
        write_all(STDOUT_FILENO, unavailable, sizeof unavailable - 1);
        _exit(1);
    }
    ssize_t length = read(fd, flag, sizeof flag - 1);
    close(fd);
    if (length > 0) {
        static const char heading[] = "The hidden poutine recipe:\n";
        write_all(STDOUT_FILENO, heading, sizeof heading - 1);
        write_all(STDOUT_FILENO, flag, (size_t)length);
        write_all(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

struct approval_gate {
    uintptr_t marker;
    uintptr_t approved;
    uintptr_t spare;
    uintptr_t backlink;
};

/* These two writable slots make safe-unlink's reciprocal-link checks explicit. */
struct approval_gate gate;

static int read_exact(int fd, void *buffer, size_t length) {
    unsigned char *cursor = buffer;
    while (length > 0) {
        ssize_t result = read(fd, cursor, length);
        if (result <= 0)
            return -1;
        cursor += (size_t)result;
        length -= (size_t)result;
    }
    return 0;
}

int main(void) {
    static const char welcome[] =
        "Poutine order desk: one previous-in-use bit is easy to overlook.\n";
    static const char card_prompt[] = "Recipe card bytes> ";
    static const char rejected[] = "The pantry links do not line up.\n";
    static const char accepted[] = "The hidden pantry opens.\n";

    unsigned char *card = malloc(0x4e0);
    void *victim = malloc(0x4e0);
    void *guard = malloc(0x100);
    if (!card || !victim || !guard)
        return 1;

    gate.marker = 0x504945524f474900ULL;
    gate.approved = (uintptr_t)card;
    gate.spare = 0;
    gate.backlink = (uintptr_t)card;

    write_all(STDOUT_FILENO, welcome, sizeof welcome - 1);
    char leak[96];
    int count = snprintf(leak, sizeof leak, "Recipe card at %p\n", (void *)card);
    if (count > 0)
        write_all(STDOUT_FILENO, leak, (size_t)count);
    write_all(STDOUT_FILENO, card_prompt, sizeof card_prompt - 1);

    /* Deliberate bug: the accepted size includes B's size byte. */
    if (read_exact(STDIN_FILENO, card, 0x4e9) != 0)
        return 1;

    free(victim);
    if (gate.approved == (uintptr_t)&gate.approved) {
        write_all(STDOUT_FILENO, accepted, sizeof accepted - 1);
        reveal_recipe();
    }
    write_all(STDOUT_FILENO, rejected, sizeof rejected - 1);
    return 0;
}
