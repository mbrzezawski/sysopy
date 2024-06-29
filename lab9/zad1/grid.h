#pragma once
#include <stdbool.h>

typedef struct {
    int index;
    int threadnum;
    char* src;
    char* dst;
} ThreadData;

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst, int row, int col);
void* thread_work(void* arg);