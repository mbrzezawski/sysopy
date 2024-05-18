#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <time.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define QUEUE_SIZE 10
#define TEXT_SIZE 11

typedef struct {
    char messages[QUEUE_SIZE][TEXT_SIZE];
    int user_ids[QUEUE_SIZE];
    int head;
    int tail;
} PrintQueue;

void semaphore_op(int sem_id, int sem_num, int op) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = op;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

void user_task(int sem_id, PrintQueue *queue, int user_id) {
    srand(time(NULL) ^ getpid());
    while (1) {
        char text[TEXT_SIZE];
        for (int i = 0; i < TEXT_SIZE - 1; i++) {
            text[i] = 'a' + rand() % 26;
        }
        text[TEXT_SIZE - 1] = '\0';

        semaphore_op(sem_id, 0, -1); // zmniejszenie liczby dostępnych miejsc w kolejce
        strcpy(queue->messages[queue->head], text);
        queue->user_ids[queue->head] = user_id;
        queue->head = (queue->head + 1) % QUEUE_SIZE;
        semaphore_op(sem_id, 1, 1); // zwiększenie liczby dostępnych zadań w kolejce

        sleep(rand() % 5 + 1); // oczekiwanie przez losowy czas
    }
}

void printer_task(int sem_id, PrintQueue *queue, int printer_id) {
    while (1) {
        semaphore_op(sem_id, 1, -1); // zmniejszenie liczby dostępnych zadań w kolejce
        char text[TEXT_SIZE];
        strcpy(text, queue->messages[queue->tail]);
        int user_id = queue->user_ids[queue->tail];
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;
        semaphore_op(sem_id, 0, 1); // zwiększenie liczby dostępnych miejsc w kolejce

        for (int i = 0; i < strlen(text); i++) {
            printf("Printer %d, User %d: %c\n", printer_id, user_id, text[i]);
            fflush(stdout);
            sleep(1); // Drukowanie znaku co sekundę
        }
        printf("Printer %d finished printing for User %d\n", printer_id, user_id);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_users> <num_printers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_users = atoi(argv[1]);
    int num_printers = atoi(argv[2]);

    int sem_id = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    semctl(sem_id, 0, SETVAL, QUEUE_SIZE); // inicjalizacja semafora miejsc w kolejce
    semctl(sem_id, 1, SETVAL, 0); // inicjalizacja semafora zadań w kolejce

    int shm_id = shmget(SHM_KEY, sizeof(PrintQueue), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        return EXIT_FAILURE;
    }

    PrintQueue *queue = (PrintQueue *)shmat(shm_id, NULL, 0);
    if (queue == (void *) -1) {
        perror("shmat failed");
        return EXIT_FAILURE;
    }
    queue->head = 0;
    queue->tail = 0;

    for (int i = 0; i < num_users; i++) {
        if (fork() == 0) {
            user_task(sem_id, queue, i);
            return EXIT_SUCCESS;
        }
    }

    for (int i = 0; i < num_printers; i++) {
        if (fork() == 0) {
            printer_task(sem_id, queue, i);
            return EXIT_SUCCESS;
        }
    }

    while (1) {
        pause();
    }
    return EXIT_SUCCESS;
}
