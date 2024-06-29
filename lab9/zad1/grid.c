#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>

const int grid_width = 30;
const int grid_height = 30;

volatile bool waiting = true;

void handle_sig(int signo){
    waiting = false;
}

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst, int row, int col)
{
    dst[row * grid_width + col] = is_alive(row, col, src);
}

void* thread_work(void* arg){
    signal(SIGCONT, handle_sig);
    ThreadData* data = (ThreadData*)arg;
    int init_row = (int)data->index/grid_width;
    int init_col = data->index - init_row * grid_width;

    while (1){
        int row = init_row;
        int col = init_col;

        while (row < grid_height){
            update_grid(data->src, data->dst, row, col);
            if ((col = col + data->threadnum) >= grid_width){
                row += 1;
                col = col - grid_width;
            }
        }
        while (waiting){
        }
        waiting = true;
    }
    return NULL;
}