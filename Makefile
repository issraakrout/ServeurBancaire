# Makefile - Compilation du projet bancaire
# Utilisation: make (compile tout), make clean (supprime les exécutables)

CC = gcc
CFLAGS = -pthread -Wall -std=c99
TARGETS = server client

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) -o server server.c
	@echo "✓ Serveur compilé avec succès!"

client: client.c
	$(CC) -o client client.c
	@echo "✓ Client compilé avec succès!"

clean:
	rm -f $(TARGETS)
	@echo "✓ Fichiers compilés supprimés"

run-server: server
	./server

run-client: client
	./client

.PHONY: all clean run-server run-client
