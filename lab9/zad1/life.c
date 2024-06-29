#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

volatile bool should_continue = true;

void handler(int signum){
    should_continue = false;
}

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("Usage: %s <threads number>", argv[0]);
        return 1;
    }

    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr(); // Start curses mode

    char *foreground = create_grid();
    char *background = create_grid();
    char *tmp;

    init_grid(foreground);

    int NUM_THREADS = atoi(argv[1]);
    if (NUM_THREADS > 28){
        printf("Threads number exceeded. Limiting threads to 28 to benefit from parallel computations.. \n");
        NUM_THREADS = 28;
    }
    pthread_t threads[NUM_THREADS];

    ThreadData* data_sets[NUM_THREADS];
    for (int i=0; i<NUM_THREADS; i++){
        data_sets[i] = malloc(sizeof(ThreadData));
        data_sets[i]->index = i;
        data_sets[i]->threadnum = NUM_THREADS;
        data_sets[i]->src = foreground;
        data_sets[i]->dst = background;
    }

    draw_grid(foreground);

    for (int i=0; i<NUM_THREADS; i++){
        pthread_create(&threads[i], NULL, thread_work, (void*)data_sets[i]);
    }
    signal(SIGINT, handler);

    while (should_continue)
    {
        draw_grid(foreground);


        usleep(500 * 1000);
        // Step simulation
        tmp = foreground;
        foreground = background;
        background = tmp;

        for (int i = 0; i < NUM_THREADS; i++) {
            data_sets[i]->src = foreground;
            data_sets[i]->dst = background;
        }

        for (int i=0; i<NUM_THREADS; i++){
            if(pthread_kill(threads[i], SIGCONT) == -1){
                perror("threadkill");
                exit(EXIT_FAILURE);
            }
        }

    }

    endwin(); // End curses mode


    destroy_grid(foreground);
    destroy_grid(background);

    for (int i=0; i<NUM_THREADS; i++){
        // if(pthread_kill(threads[i], SIGUSR1)==-1){
        // 	perror("sig threads");
        // 	exit(EXIT_FAILURE);
        // }
        free(data_sets[i]);
    }

    return 0;
}

