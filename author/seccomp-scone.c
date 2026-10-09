#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

/* The sandbox deliberately permits only the syscalls needed by the kitchen. */
static void install_sandbox(void) {
    struct sock_filter filter[] = {
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, nr)),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_read, 5, 0),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_write, 4, 0),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_openat, 3, 0),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_close, 2, 0),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_exit, 1, 0),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_exit_group, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    };
    struct sock_fprog program = {
        .len = (unsigned short)(sizeof(filter) / sizeof(filter[0])),
        .filter = filter,
    };

    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0 ||
        prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &program) != 0) {
        _exit(1);
    }
}

static void say(const char *message) {
    (void)syscall(SYS_write, STDOUT_FILENO, message, __builtin_strlen(message));
}

__attribute__((noinline, used)) static void reveal_recipe(void) {
    char flag[128];
    int fd = (int)syscall(SYS_openat, AT_FDCWD, "/flag", O_RDONLY, 0);
    if (fd < 0) {
        _exit(1);
    }
    long length = syscall(SYS_read, fd, flag, sizeof(flag));
    if (length > 0) {
        (void)syscall(SYS_write, STDOUT_FILENO, flag, (size_t)length);
    }
    (void)syscall(SYS_close, fd);
    _exit(0);
}

static void take_order(void) {
    char order[64];
    say("Send the scone recipe:\n");
    /* The recipe card is trusted even when it is longer than the counter. */
    (void)syscall(SYS_read, STDIN_FILENO, order, 256);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("== Seccomp Scone ==");
    puts("The kitchen allows a tiny syscall menu. Find a way to serve the secret recipe.");
    install_sandbox();
    for (;;) {
        take_order();
    }
}
