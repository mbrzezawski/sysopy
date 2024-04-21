CC=gcc
CFLAGS=-Wall

.PHONY: clean all integrals

all: integrals

integrals: integrals.c
	$(CC) $(CFLAGS) integrals.c -o integrals

clean:
	rm integrals times.txt