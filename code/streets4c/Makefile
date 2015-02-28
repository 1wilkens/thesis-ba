CC 		= gcc
CFLAGS	= -std=c99 -O3 -Wall -Wextra -pedantic
LDLIBS	= -losmpbf -lprotobuf-c -lz -lpthread

OBJ		= osmpbfreader.o graph.o util.o

.PHONY: streets4c test clean

all: streets4c

streets4c: main.o $(OBJ)
	$(CC) $(CFLAGS) -o streets4c main.o $(OBJ) $(LDLIBS)

test: test.o $(OBJ)
	$(CC) $(CFLAGS) -o test test.o $(OBJ) $(LDLIBS)
	./test

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o streets4c test
