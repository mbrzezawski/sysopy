CC=gcc
CFLAGS=-Wall

.PHONY: all catcher sender clean

all: catcher sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

clean:
	rm -f catcher sender
