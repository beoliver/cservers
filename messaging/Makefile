
CC=gcc-5

all: server client

server: server.c tries.c
	$(CC) -o server server.c tries.c -I.

client: client.c
	$(CC) -o client client.c
