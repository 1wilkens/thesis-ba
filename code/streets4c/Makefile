CC        = gcc
CFLAGS    = -std=c99 -O3 -g -Wall -Wextra -pedantic $(shell pkg-config --cflags glib-2.0)
LDLIBS    = -losmpbf -lprotobuf-c -lm -lz -lpthread $(shell pkg-config --libs glib-2.0)

OBJ       = osmpbfreader.o graph.o util.o

.PHONY: streets4c test clean

all: streets4c

streets4c: main.o $(OBJ)
	$(CC) $(CFLAGS) -o streets4c main.o $(OBJ) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o streets4c test
