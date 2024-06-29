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
    int sockfd;
    char name[NAME_LEN];
    int active;
} client_t;

client_t clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void send_message_to_all(char *message, char *sender);
void send_message_to_one(char *message, char *sender, char *recipient);
void list_clients(int sockfd);
void remove_client(int sockfd);
void *check_alive_clients(void *arg);
void handle_exit(int sig);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_exit);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t tid;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);
    address.sin_port = htons(atoi(argv[2]));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%s\n", argv[1], argv[2]);

    pthread_t alive_tid;
    pthread_create(&alive_tid, NULL, check_alive_clients, NULL);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) {
                clients[i].sockfd = new_socket;
                clients[i].active = 1;
                if (pthread_create(&tid, NULL, handle_client, (void *)&clients[i]) != 0) {
                    perror("pthread_create");
                }
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    return 0;
}

void *handle_client(void *arg) {
    char buffer[BUFFER_SIZE];
    char name[NAME_LEN];
    int leave_flag = 0;

    client_t *cli = (client_t *)arg;

    if (recv(cli->sockfd, name, NAME_LEN, 0) <= 0) {
        leave_flag = 1;
    } else {
        strncpy(cli->name, name, NAME_LEN);
        printf("%s has joined\n", cli->name);
    }

    while (1) {
        if (leave_flag) {
            break;
        }

        int receive = recv(cli->sockfd, buffer, BUFFER_SIZE, 0);
        if (receive > 0) {
            buffer[receive] = '\0';
            if (strncmp(buffer, "LIST", 4) == 0) {
                list_clients(cli->sockfd);
            } else if (strncmp(buffer, "2ALL ", 5) == 0) {
                send_message_to_all(buffer + 5, cli->name);
            } else if (strncmp(buffer, "2ONE ", 5) == 0) {
                char *recipient = strtok(buffer + 5, " ");
                char *message = strtok(NULL, "\n");
                send_message_to_one(message, cli->name, recipient);
            } else if (strncmp(buffer, "STOP", 4) == 0) {
                leave_flag = 1;
            }
        } else if (receive == 0 || strncmp(buffer, "STOP", 4) == 0) {
            leave_flag = 1;
        }
    }

    close(cli->sockfd);
    remove_client(cli->sockfd);
    pthread_exit(NULL);
}

void send_message_to_all(char *message, char *sender) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buffer, BUFFER_SIZE, "[%02d:%02d:%02d] %s: %s\n",
             t->tm_hour, t->tm_min, t->tm_sec, sender, message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].name, sender) != 0) {
            send(clients[i].sockfd, buffer, strlen(buffer), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_one(char *message, char *sender, char *recipient) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buffer, BUFFER_SIZE, "[%02d:%02d:%02d] %s: %s\n",
             t->tm_hour, t->tm_min, t->tm_sec, sender, message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].name, recipient) == 0) {
            send(clients[i].sockfd, buffer, strlen(buffer), 0);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void list_clients(int sockfd) {
    char buffer[BUFFER_SIZE] = "Active clients:\n";

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            strcat(buffer, clients[i].name);
            strcat(buffer, "\n");
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    send(sockfd, buffer, strlen(buffer), 0);
}

void remove_client(int sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == sockfd) {
            clients[i].active = 0;
            printf("%s has left\n", clients[i].name);
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
                if (send(clients[i].sockfd, "ALIVE", 5, 0) <= 0) {
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
            send(clients[i].sockfd, "STOP", 4, 0);
            close(clients[i].sockfd);
            clients[i].active = 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    exit(0);
}

