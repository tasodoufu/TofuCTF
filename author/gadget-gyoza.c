#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define CARD_SIZE 64
#define PLATE_KEY UINT64_C(0x47594f5a41504c41)
#define GARNISH_KEY UINT64_C(0x534553414d45504f)

static unsigned char card_length;

static int read_exact(void *destination, size_t length) {
    size_t received = 0;
    while (received < length) {
        ssize_t result = read(STDIN_FILENO, (char *)destination + received,
                              length - received);
        if (result <= 0)
            return -1;
        received += (size_t)result;
    }
    return 0;
}

__attribute__((noreturn, noinline, used))
void serve_secret_gyoza(uint64_t plate, uint64_t garnish) {
    if (plate != PLATE_KEY || garnish != GARNISH_KEY) {
        static const char rejected[] = "The order is missing its secret seasoning.\n";
        write(STDOUT_FILENO, rejected, sizeof rejected - 1);
        _exit(1);
    }

    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        static const char unavailable[] = "The secret recipe is unavailable.\n";
        write(STDOUT_FILENO, unavailable, sizeof unavailable - 1);
        _exit(1);
    }
    ssize_t length = read(fd, flag, sizeof flag - 1);
    close(fd);
    if (length > 0) {
        static const char heading[] = "Secret gyoza recipe:\n";
        write(STDOUT_FILENO, heading, sizeof heading - 1);
        write(STDOUT_FILENO, flag, (size_t)length);
        write(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

/* The gadget deliberately consumes one extra callee-saved register. */
__attribute__((naked, noinline, used))
void kitchen_gadget(void) {
    __asm__ volatile("pop %rdi\n\tpop %rsi\n\tpop %r12\n\tret");
}

/* The ROP chain needs this one-ret shim for the target's SysV stack alignment. */
__attribute__((naked, noinline, used))
void align_ret(void) {
    __asm__ volatile("ret");
}

__attribute__((noinline))
static void receive_card(void) {
    char card[CARD_SIZE];
    puts("=== Gadget Gyoza ===");
    fputs("Recipe card> ", stdout);
    if (read_exact(card, card_length) < 0) {
        puts("Incomplete recipe card.");
        return;
    }
    puts("Recipe received.");
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("A long recipe card may reach the order desk.");
    fputs("Recipe byte count (72-200)> ", stdout);
    if (read_exact(&card_length, sizeof card_length) < 0) {
        puts("Incomplete length.");
        return 1;
    }
    if (card_length < 72 || card_length > 200) {
        puts("That card length is not accepted.");
        return 1;
    }
    receive_card();
    return 0;
}
