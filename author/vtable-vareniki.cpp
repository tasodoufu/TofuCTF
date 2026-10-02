#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>

class Tray;

static void secret_recipe(Tray *);

/* A small, deliberately exposed table gives the lesson a stable fake vtable. */
static const std::uintptr_t fake_vtable[] = {
    0,
    0,
    reinterpret_cast<std::uintptr_t>(&secret_recipe),
};

class Tray {
public:
    virtual void describe();

private:
    char order[56]{};
};

void Tray::describe() {
    puts("The tray contains an ordinary order.");
}

static void secret_recipe(Tray *) {
    char flag[128]{};
    const int fd = open("/flag", O_RDONLY);
    if (fd < 0) {
        puts("The secret recipe is unavailable.");
        return;
    }
    const ssize_t length = read(fd, flag, sizeof(flag));
    if (length > 0) {
        (void)write(STDOUT_FILENO, flag, static_cast<std::size_t>(length));
    }
    close(fd);
}

__attribute__((noinline)) static void dispatch(Tray *tray) {
    tray->describe();
}

static void take_order() {
    Tray tray;

    puts("The vareniki counter accepts an order card.");
    puts("The card reader trusts its first pointer-sized field:");
    (void)syscall(SYS_read, STDIN_FILENO, &tray, 256U);
    puts("Order filed.");
    dispatch(&tray);
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    puts("== Vtable Vareniki ==");
    puts("A virtual serving method can be redirected when its object is overwritten.");
    take_order();
    return 0;
}
