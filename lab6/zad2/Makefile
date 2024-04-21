CC=gcc
CFLAGS=-Wall

.PHONY: all clean

all: catcher sender

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher

clean:
	rm catcher sender catcher_to_sender sender_to_catcher