CC=gcc
CFLAGS=-Wall

all: life

life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life -I/opt/homebrew/opt/ncurses/include -L/opt/homebrew/opt/ncurses/lib -lncursesw -lpthread

clean:
	rm -f life
