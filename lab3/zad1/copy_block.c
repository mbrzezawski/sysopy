#include <fcntl.h> // For open()
#include <unistd.h> // For close(), read(), write(), lseek()
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BLOCK_SIZE 1024

int main() {
    int source_fd, dest_fd, read_bytes;
    char buffer[BLOCK_SIZE];
    off_t file_size;
    clock_t begin = clock();

    source_fd = open("text/text_to_copy.txt", O_RDONLY);
    if (source_fd < 0) {
        perror("Error opening source file");
        return 1;
    }

    dest_fd = open("text/copied_text_block.txt", O_WRONLY | O_CREAT, 0666);
    if (dest_fd < 0) {
        perror("Error opening destination file");
        close(source_fd);
        return 1;
    }

    file_size = lseek(source_fd, 0, SEEK_END);
    if (file_size < 0) {
        perror("Error seeking in source file");
        close(source_fd);
        close(dest_fd);
        return 1;
    }

    off_t start = file_size - (file_size % BLOCK_SIZE);

    while (start >= 0) {
        if (lseek(source_fd, start, SEEK_SET) < 0) {
            perror("Error seeking in source file");
            break;
        }

        read_bytes = read(source_fd, buffer, BLOCK_SIZE);
        if (read_bytes < 0) {
            perror("Error reading source file");
            break;
        }

        for (int i = read_bytes - 1; i >= 0; i--) {
            if (write(dest_fd, &buffer[i], 1) < 0) {
                perror("Error writing to destination file");
                break;
            }
        }

        start -= BLOCK_SIZE;
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
    sprintf(measure_str, "Execution time for block: %f seconds\n", time_spent);

    if (write(measure_fd, measure_str, strlen(measure_str)) < 0) {
        perror("Error writing to measurement file");
    }

    close(measure_fd);

    return 0;
}
