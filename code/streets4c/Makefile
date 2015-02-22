CC 			?= gcc
CFLAGS		?= -std=c99 -O3 -Wall -Wextra -pedantic
LIBFLAGS	?= -pthread -lz -lprotobuf-c -losmpbf

all: streets4c.x

streets4c.x: main.c
	$(CC) $(CFLAGS) $(LIBFLAGS) main.c -o streets4c.x

clean:
	rm -rf *.o streets4c.x
