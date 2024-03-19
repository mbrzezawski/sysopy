#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main(void) {
    DIR *d;
    struct dirent *dir;
    struct stat buf;
    long long total_size = 0;

    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (stat(dir->d_name, &buf) == 0 && !S_ISDIR(buf.st_mode)) {
                printf("%s - %lld bytes\n", dir->d_name, (long long) buf.st_size);
                total_size += buf.st_size;
            }
        }
        closedir(d);
    }

    printf("Total size of files: %lld bytes\n", total_size);

    return 0;
}
