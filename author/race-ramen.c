#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static long balance = 100;
static pthread_barrier_t check_barrier;
static pthread_t workers[2];
static size_t worker_count;

static int read_line(char *buffer, size_t size) {
    if (!fgets(buffer, size, stdin))
        return 0;
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

__attribute__((noinline)) static void reveal_ramen(void) {
    FILE *fp = fopen("/flag", "r");
    char flag[128] = {0};
    if (!fp) {
        puts("The ramen recipe is missing.");
        _exit(1);
    }
    if (fgets(flag, sizeof flag, fp))
        printf("A secret ramen recipe appears: %s", flag);
    fclose(fp);
    _exit(0);
}

static void *withdraw_order(void *argument) {
    long amount = *(long *)argument;
    free(argument);

    if (amount <= 0 || amount > 100) {
        puts("That order size is not on the menu.");
        return NULL;
    }

    /* The check and the debit are separated: two orders can pass the check. */
    long observed = balance;
    if (observed < amount) {
        puts("The kitchen cannot cover that order.");
        return NULL;
    }

    /* This keeps the two stale checks together so the lesson is reproducible. */
    pthread_barrier_wait(&check_barrier);
    long remaining = __sync_sub_and_fetch(&balance, amount);
    printf("Ramen order for %ld accepted; balance is %ld.\n", amount, remaining);
    if (remaining < 0)
        reveal_ramen();
    return NULL;
}

static void menu(void) {
    puts("1) place ramen order");
    puts("2) settle pending orders");
    puts("3) inspect balance");
    puts("4) close kitchen");
    fputs("> ", stdout);
}

static void settle_orders(void) {
    for (size_t i = 0; i < worker_count; i++)
        pthread_join(workers[i], NULL);
    worker_count = 0;
    printf("All orders settled. Balance: %ld\n", balance);
}

int main(void) {
    char line[64];

    if (pthread_barrier_init(&check_barrier, NULL, 2) != 0)
        return 1;
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("=== RACE RAMEN ===");
    puts("The ramen register checks every order before debiting the bowl fund.");

    for (;;) {
        menu();
        if (!read_line(line, sizeof line))
            break;
        char *end;
        long choice = strtol(line, &end, 10);
        if (end == line || *end != '\0' || choice < 1 || choice > 4) {
            puts("That is not on the menu.");
            continue;
        }
        if (choice == 1) {
            if (worker_count >= 2) {
                puts("Settle the two pending orders first.");
                continue;
            }
            fputs("Order amount: ", stdout);
            if (!read_line(line, sizeof line))
                break;
            long *amount = malloc(sizeof *amount);
            if (!amount)
                return 1;
            *amount = strtol(line, &end, 10);
            if (end == line || *end != '\0') {
                free(amount);
                puts("That is not an order amount.");
                continue;
            }
            if (pthread_create(&workers[worker_count], NULL, withdraw_order, amount) != 0) {
                free(amount);
                puts("The kitchen is too busy.");
                continue;
            }
            worker_count++;
            puts("Order queued.");
        } else if (choice == 2) {
            settle_orders();
        } else if (choice == 3) {
            printf("Current balance: %ld\n", balance);
        } else {
            settle_orders();
            break;
        }
    }
    pthread_barrier_destroy(&check_barrier);
    return 0;
}
