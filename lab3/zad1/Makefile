CC=gcc
CFLAGS=-Wall

.PHONY: all reverse_by_block reverse_by_byte clean

all: reverse_by_byte reverse_by_block

reverse_by_byte: copy.c
	$(CC) $(CFLAGS) copy.c -o reverse_by_byte

reverse_by_block: copy_block.c
	$(CC) $(CFLAGS) copy_block.c -o reverse_by_block

clean:
	rm -f reverse_by_byte reverse_by_block pomiar_zad_1.txt
