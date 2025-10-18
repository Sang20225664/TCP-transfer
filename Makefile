all: server client

server:
	$(CC) TCP_Server/server.c TCP_Server/file_receiver.c TCP_Server/logger/logger.c TCP_Server/validation/validation.c -o server

client:
	$(CC) TCP_Client/client.c TCP_Client/file_transfer.c TCP_Client/validation.c -o client

clean:
	rm -f server client
