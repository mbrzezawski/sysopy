#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define NAME_LEN 32

volatile int sockfd;

void *receive_handler(void *arg);
void handle_exit(int sig);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <name> <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *name = argv[1];
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    struct sockaddr_in server_addr;

    signal(SIGINT, handle_exit);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    send(sockfd, name, NAME_LEN, 0);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    char buffer[BUFFER_SIZE];

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        if (strncmp(buffer, "STOP", 4) == 0) {
            break;
        }
    }

    handle_exit(0);
    return 0;
}

void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    int receive;

    while ((receive = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[receive] = '\0';
        printf("%s\n", buffer);
    }

    return NULL;
}

void handle_exit(int sig) {
    send(sockfd, "STOP", 4, 0);
    close(sockfd);
    exit(0);
}
