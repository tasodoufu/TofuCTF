#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Small, explicit gadgets keep the lesson focused on leaking libc, not gadget hunting. */
__asm__(
    ".global tofu_pop_rdi\n"
    "tofu_pop_rdi:\n"
    "    pop %rdi\n"
    "    ret\n"
    ".global tofu_ret\n"
    "tofu_ret:\n"
    "    ret\n"
);

static ssize_t legacy_read(void *destination, size_t count) {
    return read(STDIN_FILENO, destination, count);
}

static void recipe_counter(void) {
    char recipe[64];

    puts("The lasagna counter accepts a 64-byte recipe card.");
    puts("Write the recipe and finish with a newline:");
    (void)legacy_read(recipe, 256U);
    puts("Recipe accepted.");
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== Leak Lasagna ==");
    puts("The chef prints one familiar address before serving the next order.");
    recipe_counter();
    return 0;
}
