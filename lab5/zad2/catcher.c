#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int sender_pid = 0;
int change_requests = 0;

void handle_sigusr1(int sig, siginfo_t *info, void *context) {
    sender_pid = info->si_pid;
    int mode = info->si_value.sival_int;

    if (mode == 1 || mode == 2) {
        if (mode == 1) {
            change_requests++;
            for (int i = 1; i <= 100; i++) printf("%d\n", i);
        }
        else {
            change_requests++;
            printf("Change requests: %d\n", change_requests);
        }

        if (sender_pid != 0) {
            kill(sender_pid, SIGUSR1);
        }
    }
    else if (mode == 3) {
        if (sender_pid != 0) {
            kill(sender_pid, SIGUSR1);
        }

        printf("Exiting...\n");
        exit(0);
    }
}

int main() {
    printf("Catcher PID: %d\n", getpid());

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);

    while (1) {
        pause();
    }

    return 0;
}
