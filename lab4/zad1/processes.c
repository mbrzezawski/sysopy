#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of child processes>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Number of child processes must be a positive integer.\n");
        return 1;
    }

    printf("Parent PID: %d\n", getpid());

    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            return 1;
        }

        // Child process
        if (pid == 0) {
            printf("Parent PID: %d, Child PID: %d\n", getppid(), getpid());
            return 0;
        }
    }
    for (int i = 0; i < n; ++i) {
        wait(NULL);
    }

    printf("%s\n", argv[1]);
    return 0;
}
