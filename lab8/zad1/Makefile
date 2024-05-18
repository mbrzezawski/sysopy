CC=gcc
CFLAGS=-Wall

.PHONY: all main clean test

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f main

test: main
	./main 10 3
