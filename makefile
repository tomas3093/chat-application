all: server client

clean:
	rm -f client server

server: Server/server.c Server/server.h konstanty.h routines.h
	gcc -o Release/server Server/server.c -lpthread

client: Client/client.c Client/client.h konstanty.h routines.h
	gcc -o Release/client Client/client.c
