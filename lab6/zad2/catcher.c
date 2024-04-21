#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

double function(double rectangle_center){
    return 4.0/(rectangle_center*rectangle_center + 1);
}

double integrate(double start, double end, int precision){
    double rectangle_width = (double)(end - start)/precision;
    if (rectangle_width == 0){
        printf("Error with rectangle width...\n");
        exit(EXIT_FAILURE);
    }
    double partial;
    double result = 0.0;

    for (int i=0;i<precision;i++){
        partial = function(rectangle_width/2.0 + i*rectangle_width) * rectangle_width;
        result += partial;
    }
    return result;
}




int main(int argc, char *argv[]){
    if (argc != 2){printf("Usage: %s, <precision of integration>\n", argv[0]); return 1;}
    int precision = atoi(argv[1]);
    if(precision == 0){
        printf("Error with calculating integral: incorrect precision!\n");
        exit(EXIT_FAILURE);
    }

    const char *catcher_to_sender = "catcher_to_sender";
    if (mkfifo(catcher_to_sender, 0666) == -1) {perror("mkfifo - catcher"); return 1;}

    const char *sender_to_catcher = "sender_to_catcher";
    if (mkfifo(sender_to_catcher, 0666) == -1) {perror("mkfifo - sender"); return 1;}

    int catching_pipe;
    if((catching_pipe = open(sender_to_catcher, O_RDONLY)) == -1){perror("open catching");return 1;};

    int sending_pipe;
    if((sending_pipe = open(catcher_to_sender, O_WRONLY)) == -1){
        perror("open sending");
        close(catching_pipe);
        return 1;
    }

    double values[2];
    if (read(catching_pipe, values, 2*sizeof(double)) == -1) {
        perror("read");
        close(catching_pipe);
        close(sending_pipe);
        return 1;
    }

    double result = integrate(values[0], values[1], precision);

    if(write(sending_pipe, &result, sizeof(double)) == -1){
        perror("write");
        close(catching_pipe);
        close(sending_pipe);
        return 1;
    }

    close(catching_pipe);
    close(sending_pipe);

    return 0;
}