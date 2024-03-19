CC=gcc
CFLAGS=-Wall

.PHONY: all file_sizes clean

all: file_sizes

file_sizes: file_sizes.c
	$(CC) $(CFLAGS) file_sizes.c -o file_sizes

clean:
	rm -f file_sizes

