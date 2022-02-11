compiler=gcc

server: server.o server_backend.o list.o list_clients.o list_groups.o list_shared_files.o
	$(compiler) $? -pthread -o $@

server.o: server.c
	$(compiler) -c $? -o $@

server_backend.o: server_backend.h server_backend.c
	$(compiler) -c server_backend.c -o $@

list_clients.o: list_clients.h list_clients.c
	$(compiler) -c list_clients.c -o $@

list_groups.o: list_groups.h list_groups.c
	$(compiler) -c list_groups.c -o $@

list_shared_files.o: list_shared_files.h list_shared_files.c
	$(compiler) -c list_shared_files.c -o $@

list.o: list.h list.c
	$(compiler) -c list.c -o $@

clean:
	rm *.o
