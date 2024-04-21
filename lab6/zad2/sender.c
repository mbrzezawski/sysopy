#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if (argc != 1){
        printf("Usage: %s\n", argv[0]);
        return 1;
    }


    const char *catcher_to_sender = "catcher_to_sender";
    const char *sender_to_catcher = "sender_to_catcher";
    int catching_pipe;

    int sending_pipe;
    if((sending_pipe = open(sender_to_catcher, O_WRONLY)) == -1){
        perror("open sending");
        return 1;
    }

    if((catching_pipe = open(catcher_to_sender, O_RDONLY)) == -1){
        perror("open catching");
        close(sending_pipe);
        return 1;
    };

    double value1, value2;

    if (scanf("%lf %lf", &value1, &value2) != 2) {
        printf("Error reading input\n");
        close(catching_pipe);
        close(sending_pipe);
        return 1;
    }

    if (write(sending_pipe, &value1, sizeof(double)) == -1) {
        perror("write");
        close(catching_pipe);
        close(sending_pipe);
        return 1;
    }
    if (write(sending_pipe, &value2, sizeof(double)) == -1) {
        perror("write");
        close(catching_pipe);
        close(sending_pipe);
        return 1;
    }


    double value;

    if(read(catching_pipe, &value, sizeof(double)) == -1){
        perror("read");
        close(catching_pipe);
        close(sending_pipe);
        return 1;
    }

    printf("Integrated value: %f\n", value);

    close(sending_pipe);
    close(catching_pipe);

    return 0;
}