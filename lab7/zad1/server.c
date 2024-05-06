#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256
#define SERVER_QUEUE_KEY 1234

struct message {
    long type;
    int client_id;
    char text[MAX_MSG_SIZE];
};

struct client_info {
    int client_queue_id;
    int client_id;
};

struct client_info clients[MAX_CLIENTS];
int num_clients = 0;

int find_client_index(int client_id) {
    for (int i = 0; i < num_clients; ++i) {
        if (clients[i].client_id == client_id) {
            return i;
        }
    }
    return -1;
}

int main() {
    int server_queue = msgget(SERVER_QUEUE_KEY, IPC_CREAT | 0666);
    if (server_queue == -1) {
        perror("Server: msgget");
        exit(1);
    }

    struct message msg;
    while (1) {
        if (msgrcv(server_queue, &msg, sizeof(msg) - sizeof(long), 0, 0) != -1) {
            if (strcmp(msg.text, "INIT") == 0) {
                if (num_clients < MAX_CLIENTS) {
                    key_t client_key = ftok(".", msg.client_id);
                    clients[num_clients].client_queue_id = msgget(client_key, 0666);
                    clients[num_clients].client_id = msg.client_id;

                    if (clients[num_clients].client_queue_id == -1) {
                        perror("Server: msgget client queue");
                        exit(1);
                    }

                    msg.type = 1;
                    snprintf(msg.text, MAX_MSG_SIZE, "Welcome Client %d", msg.client_id);

                    if (msgsnd(clients[num_clients].client_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
                        perror("Server: msgsnd INIT response");
                        exit(1);
                    }

                    num_clients++;
                } else {
                    printf("Maximum number of clients reached. Ignoring INIT message.\n");
                }
            } else {
                printf("Received message from client %d: %s\n", msg.client_id, msg.text);
                for (int i = 0; i < num_clients; ++i) {
                    if (clients[i].client_queue_id != -1 && clients[i].client_id != msg.client_id) {
                        if (msgsnd(clients[i].client_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
                            perror("Server: msgsnd to client");
                        }
                    }
                }
            }
        }
        usleep(100000);
    }

    msgctl(server_queue, IPC_RMID, NULL);
    return 0;
}
