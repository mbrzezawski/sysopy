#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigusr1(int sig) {
    printf("Signal acknowledged by catcher\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <catcher_pid> <mode>\n", argv[0]);
        return 1;
    }

    int catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    signal(SIGUSR1, handle_sigusr1);

    union sigval value;
    value.sival_int = mode;
    sigqueue(catcher_pid, SIGUSR1, value);

    pause();

    return 0;
}

