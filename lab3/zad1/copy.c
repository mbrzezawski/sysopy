#include <fcntl.h> // For open()
#include <unistd.h> // For close(), read(), write(), lseek()
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 1

int main() {
    int source_fd, dest_fd, read_bytes;
    char buffer[BUFFER_SIZE];
    clock_t begin = clock();

    source_fd = open("text/text_to_copy.txt", O_RDONLY);
    if (source_fd < 0) {
        perror("Error opening source file");
        return 1;
    }

    dest_fd = open("text/copied_text_byte.txt", O_WRONLY | O_CREAT, 0666);
    if (dest_fd < 0) {
        perror("Error opening destination file");
        close(source_fd);
        return 1;
    }

    off_t file_size = lseek(source_fd, 0, SEEK_END);
    if (file_size < 0) {
        perror("Error seeking in source file");
        close(source_fd);
        close(dest_fd);
        return 1;
    }

    for(off_t i = 1; i <= file_size; i++) {
        if (lseek(source_fd, -i, SEEK_END) < 0) {
            perror("Error seeking in source file");
            break;
        }

        read_bytes = read(source_fd, buffer, BUFFER_SIZE);
        if (read_bytes < 0) {
            perror("Error reading source file");
            break;
        }

        if (write(dest_fd, buffer, read_bytes) < 0) {
            perror("Error writing to destination file");
            break;
        }
    }

    close(source_fd);
    close(dest_fd);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    int measure_fd = open("pomiar_zad_1.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (measure_fd < 0) {
        perror("Error opening measurement file");
        close(source_fd);
        close(dest_fd);
        return 1;
    }

    char measure_str[100];
    sprintf(measure_str, "Execution time for byte: %f seconds\n", time_spent);

    if (write(measure_fd, measure_str, strlen(measure_str)) < 0) {
        perror("Error writing to measurement file");
    }

    close(measure_fd);

    return 0;
}
