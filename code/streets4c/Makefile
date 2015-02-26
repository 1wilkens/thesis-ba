CC 		= gcc
CFLAGS	= -std=c99 -O3 -Wall -Wextra -pedantic
LDLIBS	= -losmpbf -lprotobuf-c -lz -lpthread

.PHONY: streets4c clean

all: streets4c

streets4c: main.c
	$(CC) $(CFLAGS) main.c $(LDLIBS) -o streets4c

clean:
	rm -rf *.o streets4c
