#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void signal_handler(int signum) {
    printf("Received SIGUSR1 signal with signal number %d\n", signum);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <none|ignore|handler|mask>\n", argv[0]);
        return 1;
    }

    if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) {
        perror("Can't set default signal handler");
    }

    if (strcmp(argv[1], "none") == 0) {
    }
    else if (strcmp(argv[1], "ignore") == 0) {
        if (signal(SIGUSR1, SIG_IGN) == SIG_ERR) {
            perror("Can't ignore SIGUSR1");
        }
    }
    else if (strcmp(argv[1], "handler") == 0) {
        if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
            perror("Can't set signal handler");
        }
    }
    else if (strcmp(argv[1], "mask") == 0) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            perror("Can't block SIGUSR1");
        }
    }
    else {
        fprintf(stderr, "Invalid argument '%s'. Use none, ignore, handler, or mask.\n", argv[1]);
        return 1;
    }

    raise(SIGUSR1);

    if (strcmp(argv[1], "mask") == 0) {
        sigset_t pending;
        sigemptyset(&pending);
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("SIGUSR1 signal is pending.\n");
        }
        else {
            printf("SIGUSR1 signal is not pending.\n");
        }
    }
    pause();

    return 0;
}
