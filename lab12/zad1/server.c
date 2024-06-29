#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NAME_LEN 32

typedef struct {
    struct sockaddr_in addr;
    char name[NAME_LEN];
    int active;
} client_t;

client_t clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_fd;

void send_message_to_all(char *message, struct sockaddr_in *sender_addr);
void send_message_to_one(char *message, struct sockaddr_in *sender_addr, char *recipient);
void list_clients(struct sockaddr_in *client_addr);
void remove_client(struct sockaddr_in *client_addr);
void *check_alive_clients(void *arg);
void handle_exit(int sig);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_exit);

    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%s\n", argv[1], argv[2]);

    pthread_t alive_tid;
    pthread_create(&alive_tid, NULL, check_alive_clients, NULL);

    char buffer[BUFFER_SIZE];

    while (1) {
        int len = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[len] = '\0';

        pthread_mutex_lock(&clients_mutex);
        int client_exists = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && clients[i].addr.sin_addr.s_addr == client_addr.sin_addr.s_addr && clients[i].addr.sin_port == client_addr.sin_port) {
                client_exists = 1;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        if (!client_exists) {
            if (strncmp(buffer, "STOP", 4) != 0) {
                pthread_mutex_lock(&clients_mutex);
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (!clients[i].active) {
                        clients[i].addr = client_addr;
                        clients[i].active = 1;
                        strncpy(clients[i].name, buffer, NAME_LEN);
                        printf("%s has joined\n", clients[i].name);
                        break;
                    }
                }
                pthread_mutex_unlock(&clients_mutex);
            }
        } else {
            if (strncmp(buffer, "LIST", 4) == 0) {
                list_clients(&client_addr);
            } else if (strncmp(buffer, "2ALL ", 5) == 0) {
                send_message_to_all(buffer + 5, &client_addr);
            } else if (strncmp(buffer, "2ONE ", 5) == 0) {
                char *recipient = strtok(buffer + 5, " ");
                char *message = strtok(NULL, "\n");
                send_message_to_one(message, &client_addr, recipient);
            } else if (strncmp(buffer, "STOP", 4) == 0) {
                remove_client(&client_addr);
            }
        }
    }

    return 0;
}

void send_message_to_all(char *message, struct sockaddr_in *sender_addr) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buffer, BUFFER_SIZE, "[%02d:%02d:%02d] %s: %s\n",
             t->tm_hour, t->tm_min, t->tm_sec, inet_ntoa(sender_addr->sin_addr), message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && (clients[i].addr.sin_addr.s_addr != sender_addr->sin_addr.s_addr || clients[i].addr.sin_port != sender_addr->sin_port)) {
            sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_one(char *message, struct sockaddr_in *sender_addr, char *recipient) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buffer, BUFFER_SIZE, "[%02d:%02d:%02d] %s: %s\n",
             t->tm_hour, t->tm_min, t->tm_sec, inet_ntoa(sender_addr->sin_addr), message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].name, recipient) == 0) {
            sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void list_clients(struct sockaddr_in *client_addr) {
    char buffer[BUFFER_SIZE] = "Active clients:\n";

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            strcat(buffer, clients[i].name);
            strcat(buffer, "\n");
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}

void remove_client(struct sockaddr_in *client_addr) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].addr.sin_addr.s_addr == client_addr->sin_addr.s_addr && clients[i].addr.sin_port == client_addr->sin_port) {
            printf("%s has left\n", clients[i].name);
            clients[i].active = 0;
            clients[i].name[0] = '\0';
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *check_alive_clients(void *arg) {
    while (1) {
        sleep(10);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                if (sendto(server_fd, "ALIVE", 5, 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr)) <= 0) {
                    clients[i].active = 0;
                    printf("%s has been disconnected\n", clients[i].name);
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}

void handle_exit(int sig) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            sendto(server_fd, "STOP", 4, 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
            clients[i].active = 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    exit(0);
}

