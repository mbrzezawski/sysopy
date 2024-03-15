CC := gcc
CFLAGS := -Wall
LDFLAGS := -Lcollatzlibrary
LDLIBS_DYNAMIC := -ldl
SRC := client.c
EXE := client

.PHONY: all static shared dynamic clean

all: static shared dynamic

static:
	$(CC) $(CFLAGS) $(SRC) -o $(EXE)_static $(LDFLAGS) -lcollatz -Wl,-rpath,$(PWD)/collatzlibrary

shared:
	$(CC) $(CFLAGS) $(SRC) -o $(EXE)_shared $(LDFLAGS) -lcollatz -Wl,-rpath,$(PWD)/collatzlibrary

dynamic:
	$(CC) $(CFLAGS) -DDYNAMIC_DLOPEN $(SRC) -o $(EXE)_dynamic $(LDFLAGS) $(LDLIBS_DYNAMIC)

clean:
	rm -f $(EXE)_static $(EXE)_shared $(EXE)_dynamic
