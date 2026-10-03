#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct Recipe Recipe;
typedef struct Node Node;

typedef void (*serve_fn)(void);

struct Recipe {
    serve_fn serve;
};

struct Node {
    char note[32];
    Node *prev;
    Node *next;
};

static Node *head;
static Recipe *current_recipe;
static Recipe normal_recipe;
static Recipe fake_recipe;

static ssize_t read_card(void *address, size_t length) {
    return read(STDIN_FILENO, address, length);
}

static void ordinary_recipe(void) {
    puts("The ordinary recipe is served.");
}

static void secret_recipe(void) {
    char flag[128] = {0};
    int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The secret recipe is unavailable.");
        return;
    }
    ssize_t length = read(fd, flag, sizeof(flag) - 1U);
    if (length > 0) {
        (void)write(STDOUT_FILENO, flag, (size_t)length);
    }
    close(fd);
}

static void setup_recipes(void) {
    normal_recipe.serve = ordinary_recipe;
    fake_recipe.serve = secret_recipe;
    current_recipe = &normal_recipe;
}

static void add_order(void) {
    Node *node = calloc(1U, sizeof(*node));
    if (node == NULL) {
        puts("The order book is full.");
        return;
    }
    node->next = head;
    if (head != NULL) {
        head->prev = node;
    }
    head = node;
    puts("Order added.");
}

static void edit_order(void) {
    if (head == NULL) {
        puts("There is no order to edit.");
        return;
    }
    puts("Write the order note:");
    /* The note is 32 bytes, but the reader trusts a larger card. */
    void *card = (void *)(uintptr_t)(void *)head->note;
    (void)read_card(card, 64U);
    puts("Order updated.");
}

static void unlink_order(Node *victim) {
    /* Deliberately missing pointer-consistency validation: unsafe unlink. */
    if (victim->prev != NULL) {
        victim->prev->next = victim->next;
    } else {
        head = victim->next;
    }
    if (victim->next != NULL && victim->next->prev == victim) {
        victim->next->prev = victim->prev;
    }
}

static void remove_order(void) {
    if (head == NULL) {
        puts("There is no order to remove.");
        return;
    }
    Node *victim = head;
    unlink_order(victim);
    free(victim);
    head = NULL;
    puts("Order removed.");
}

static void serve_recipe(void) {
    puts("Serving the selected recipe...");
    current_recipe->serve();
}

static void menu(void) {
    char choice[8] = {0};
    for (;;) {
        puts("\n1. Add order");
        puts("2. Edit order");
        puts("3. Remove order");
        puts("4. Serve recipe");
        puts("5. Exit");
        fputs("> ", stdout);
        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            return;
        }
        switch (choice[0]) {
        case '1':
            add_order();
            break;
        case '2':
            edit_order();
            break;
        case '3':
            remove_order();
            break;
        case '4':
            serve_recipe();
            return;
        case '5':
            return;
        default:
            puts("Unknown order.");
            break;
        }
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setup_recipes();
    puts("== Unlink Ube ==");
    puts("The order book links neighboring orders without checking their pointers.");
    menu();
    return 0;
}
