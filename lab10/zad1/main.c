#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_REINDEER 9

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_ready = PTHREAD_COND_INITIALIZER;

int reindeer_count = 0;

void* reindeer_thread(void* id) {
    int reindeer_id = *((int*)id);
    free(id);

    while (1) {
        sleep((rand() % 6) + 5);

        pthread_mutex_lock(&reindeer_mutex);
        reindeer_count++;
        printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n", reindeer_count, reindeer_id);

        if (reindeer_count == NUM_REINDEER) {
            printf("Renifer: wybudzam Mikołaja, ID: %d\n", reindeer_id);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_signal(&santa_cond);
            pthread_mutex_unlock(&santa_mutex   );
        }

        pthread_cond_wait(&reindeer_ready, &reindeer_mutex);

        pthread_mutex_unlock(&reindeer_mutex);
    }
    return NULL;
}


void* santa_thread(void* arg) {
    int delivery_count = 0;
    while (delivery_count < 4) {
        pthread_mutex_lock(&santa_mutex);
        while (reindeer_count < NUM_REINDEER) {
            pthread_cond_wait(&santa_cond, &santa_mutex);
        }

        printf("Mikołaj: budzę się\n");
        printf("Mikołaj: dostarczam zabawki\n");
        sleep((rand() % 3) + 2);

        reindeer_count = 0;
        pthread_mutex_unlock(&santa_mutex);

        pthread_cond_broadcast(&reindeer_ready);

        printf("Mikołaj: zasypiam\n");
        delivery_count++;
    }
    return NULL;
}


int main() {
    srand(time(NULL));
    pthread_t santa;
    pthread_t reindeers[NUM_REINDEER];

    pthread_create(&santa, NULL, santa_thread, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&reindeers[i], NULL, reindeer_thread, id);
    }

    pthread_join(santa, NULL);
    for (int i = 0; i < NUM_REINDEER; i++) {
        pthread_cancel(reindeers[i]);
        pthread_join(reindeers[i], NULL);
    }

    pthread_mutex_destroy(&santa_mutex);
    pthread_mutex_destroy(&reindeer_mutex);
    pthread_cond_destroy(&santa_cond);

    return 0;
}