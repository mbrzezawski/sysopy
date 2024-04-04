#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int global = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory path>\n", argv[0]);
        return 1;
    }
    int local = 0;

    printf("%s\n", argv[0]);
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    }
    else if (pid == 0) {
        // Child process
        printf("child process\n");
        local++;
        global++;

        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);

        int exec_status = execl("/bin/ls", "ls", argv[1], NULL);

        if (exec_status == -1) {
            printf("Error code: %d\n", errno);
            return 1;
        }
    }
    else {
        // Parent process
        int status;
        printf("parent process\n");
        waitpid(pid, &status, 0);

        printf("parent pid = %d, child pid = %d\n", getpid(), pid);

        if(WIFEXITED(status)) {
            printf("Child exit code: %d\n", WEXITSTATUS(status));
        }

        printf("Parent's local = %d, parent's global = %d\n", local, global);

        return WEXITSTATUS(status);
    }
    return 0;
}
