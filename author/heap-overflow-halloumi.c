#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct serving_station {
    void (*serve)(void);
};

static void reject_order(void) {
    static const char message[] = "The serving station rejects this batch.\n";
    write(STDOUT_FILENO, message, sizeof message - 1);
    _exit(0);
}

__attribute__((used, noinline, noreturn))
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
        static const char heading[] = "The secret recipe:\n";
        write(STDOUT_FILENO, heading, sizeof heading - 1);
        write(STDOUT_FILENO, flag, (size_t)length);
        write(STDOUT_FILENO, "\n", 1);
    }
    _exit(0);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    static const char count_prompt[] = "Count> ";
    static const char recipe_prompt[] = "Recipe> ";
    static const char invalid[] = "Invalid batch count.\n";
    static const char baked[] = "Batch prepared.\n";
    char line[32] = {0};

    write(STDOUT_FILENO, count_prompt, sizeof count_prompt - 1);
    ssize_t got = read(STDIN_FILENO, line, sizeof line - 1);
    if (got <= 0)
        return 1;

    char *end = NULL;
    unsigned long parsed = strtoul(line, &end, 10);
    if (end == line || parsed < 4096 || parsed > UINT16_MAX) {
        write(STDOUT_FILENO, invalid, sizeof invalid - 1);
        return 1;
    }
    uint16_t count = (uint16_t)parsed;

    /* BUG: narrowing the full byte count makes this allocation too small. */
    uint16_t allocation_size = (uint16_t)(count * sizeof(uint64_t));
    char *recipe = malloc(allocation_size);
    struct serving_station *station = malloc(sizeof *station);
    if (!recipe || !station)
        return 1;
    station->serve = reject_order;

    write(STDOUT_FILENO, recipe_prompt, sizeof recipe_prompt - 1);
    /* BUG: copy uses the untruncated size, not allocation_size. */
    (void)read(STDIN_FILENO, recipe, (size_t)count * sizeof(uint64_t));
    write(STDOUT_FILENO, baked, sizeof baked - 1);
    station->serve();
    return 0;
}
