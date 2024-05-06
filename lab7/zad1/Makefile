CC=gcc
CFLAGS=-Wall

.PHONY: all server client clean

all: server.c client.c
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFlAGS) client.c -o client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f server client