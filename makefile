CC=gcc
CFLAGS=-Wall

all: server client

server: server.c protocol.c
	$(CC) $(CFLAGS) -o server server.c protocol.c

client: client.c protocol.c
	$(CC) $(CFLAGS) -o client client.c protocol.c

clean:
	rm -f server client *.o

.PHONY: all clean