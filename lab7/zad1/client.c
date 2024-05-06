#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MSG_SIZE 256
#define SERVER_QUEUE_KEY 1234

struct message {
    long type;
    int client_id;
    char text[MAX_MSG_SIZE];
};

int main() {
    key_t client_key = ftok(".", getpid());

    int client_queue = msgget(client_key, IPC_CREAT | 0666);
    if (client_queue == -1) {
        perror("Client: msgget client queue");
        exit(1);
    }

    int server_queue = msgget(SERVER_QUEUE_KEY, 0666);
    if (server_queue == -1) {
        perror("Client: msgget server queue");
        exit(1);
    }

    struct message init_msg = {1, getpid(), "INIT"};
    if (msgsnd(server_queue, &init_msg, sizeof(init_msg) - sizeof(long), 0) == -1) {
        perror("Client: msgsnd INIT");
        exit(1);
    }

    printf("Client %d connected to server.\n", getpid());

    if (fork() == 0) {
        struct message msg;
        while (1) {
            if (msgrcv(client_queue, &msg, sizeof(msg) - sizeof(long), 1, 0) != -1) {
                printf("Received message from server: %s\n", msg.text);
            }
            usleep(100000);
        }
        exit(0);
    }

    char input[MAX_MSG_SIZE];
    printf("Enter message to send to server (or 'exit' to quit):\n");
    while (1) {
        fgets(input, MAX_MSG_SIZE, stdin);
        if (strncmp(input, "exit", 4) == 0) {
            break;
        }

        struct message msg = {1, getpid(), ""};
        strncpy(msg.text, input, MAX_MSG_SIZE);

        if (msgsnd(server_queue, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("Client: msgsnd message");
            exit(1);
        }
    }

    msgctl(client_queue, IPC_RMID, NULL);
    return 0;
}
