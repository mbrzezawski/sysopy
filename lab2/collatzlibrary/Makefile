CC = gcc
CFLAGS = -Wall -Werror -fpic
SOURCE_FILES = collatz.c
OBJECT_FILES = $(SOURCE_FILES:.c=.o)
STATIC_LIBRARY = libcollatz.a
SHARED_LIBRARY = libcollatz.so

.PHONY: all static shared clean

all: static shared

static: $(STATIC_LIBRARY)

shared: $(SHARED_LIBRARY)

$(STATIC_LIBRARY): $(OBJECT_FILES)
	ar rcs $(STATIC_LIBRARY) $(OBJECT_FILES)

$(SHARED_LIBRARY): $(OBJECT_FILES)
	$(CC) -shared -o $(SHARED_LIBRARY) $(OBJECT_FILES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECT_FILES) $(STATIC_LIBRARY) $(SHARED_LIBRARY)

