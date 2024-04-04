CC=gcc
CFLAGS=-Wall

.PHONY: all process_creation clean

all: process_creation

process_creation: processes_dic.c
	$(CC) $(CFLAGS) processes_dic.c -o processes_dic

clean:
	rm -f processes_dic
