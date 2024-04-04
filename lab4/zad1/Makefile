CC=gcc
CFLAGS=-Wall

.PHONY: all process_creation clean

all: process_creation

process_creation: processes.c
	$(CC) $(CFLAGS) processes.c -o processes

clean:
	rm -f processes
