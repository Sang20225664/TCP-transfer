all: server client

server: TCP_Server/server.c \
        TCP_Server/message/message.c \
        TCP_Server/receiver/file_receiver.c \
        TCP_Server/validation/validation.c \
        TCP_Server/logger/logger.c
	cc $^ -o server

client:
	$(CC) TCP_Client/client.c TCP_Client/file_transfer.c TCP_Client/validation.c -o client

clean:
	rm -f server client
