#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define AUTHORIZED 0xC0FFEE42u

struct kitchen_ticket {
    char note[24];
    uint32_t authorization;
};

static int read_exact(int fd, void *buffer, size_t length) {
    unsigned char *cursor = buffer;
    size_t received = 0;
    while (received < length) {
        ssize_t result = read(fd, cursor + received, length - received);
        if (result <= 0)
            return -1;
        received += (size_t)result;
    }
    return 0;
}

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
static void reveal_recipe(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        static const char unavailable[] = "The secret recipe is unavailable.\n";
        write_all(STDOUT_FILENO, unavailable, sizeof unavailable - 1);
        _exit(1);
    }
    ssize_t length = read(fd, flag, sizeof flag - 1);
    close(fd);
    if (length > 0) {
        static const char heading[] = "Shakshuka's secret recipe:\n";
        write_all(STDOUT_FILENO, heading, sizeof heading - 1);
        write_all(STDOUT_FILENO, flag, (size_t)length);
        write_all(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    struct kitchen_ticket ticket = {0};
    int16_t signed_length = 0;

    static const char title[] = "=== Signed Shakshuka ===\n";
    static const char length_prompt[] = "Signed length (2 little-endian bytes)> ";
    static const char note_prompt[] = "Recipe note> ";
    write_all(STDOUT_FILENO, title, sizeof title - 1);
    write_all(STDOUT_FILENO, length_prompt, sizeof length_prompt - 1);
    if (read_exact(STDIN_FILENO, &signed_length, sizeof signed_length) < 0) {
        write_all(STDOUT_FILENO, "Incomplete length.\n", 19);
        return 1;
    }

    if (signed_length > (int16_t)sizeof ticket.note) {
        write_all(STDOUT_FILENO, "That note is too long.\n", 23);
        return 0;
    }
    write_all(STDOUT_FILENO, "Length accepted.\n", 17);
    write_all(STDOUT_FILENO, note_prompt, sizeof note_prompt - 1);

    /* Bug: -1 becomes 65535 as uint16_t, then widens to size_t. */
    size_t requested = (size_t)(uint16_t)signed_length;
    ssize_t received = read(STDIN_FILENO, ticket.note, requested);
    if (received < 0) {
        write_all(STDOUT_FILENO, "Could not read the note.\n", 25);
        return 1;
    }
    if (ticket.authorization == AUTHORIZED)
        reveal_recipe();

    write_all(STDOUT_FILENO, "The kitchen keeps this ordinary recipe.\n",
              sizeof "The kitchen keeps this ordinary recipe.\n" - 1);
    return 0;
}
