#include "server_backend.h"

typedef struct
{
	struct pollfd fds[MAXN_FD-1];
	size_t nfds;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}stack_t;

list_t * clients;
list_t * groups;
stack_t stack = {.nfds = 0, .mutex = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER};

void * server_subroutine(void * arg)
{
	struct pollfd client;

	while(true)
	{
		//get a client
		pthread_mutex_lock(&stack.mutex);

		while(stack.nfds == 0)
		{
			pthread_cond_wait(&stack.cond,&stack.mutex);
		}

		stack.nfds--;
		client = stack.fds[stack.nfds];

		pthread_mutex_unlock(&stack.mutex);

		if(poll(&client,1,-1) != -1)
		{
			if(client.revents == POLLERR || client.revents == 0)
			{
				close(client.fd);
				continue;
			}

			char buffer[100];
			if(recv(client.fd, buffer, sizeof(buffer),0) == 1 && find_node(clients, buffer) != NULL)
			{
				
			}
			
		}
	}

	puts("poll() failed. (subroutine)");
	exit(EXIT_FAILURE);
	return NULL;
}

void * server_routine(void * arg)
{
	struct sockaddr_in sockaddr;
	struct pollfd server_sockfd;

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(8081);
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	for(size_t i = 0; i < 8; i++)
	{
		sockaddr.sin_zero[i] = 0;
	}

	server_sockfd.fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_sockfd.fd == -1)
	{
		perror("ERROR: socket() failed.");
		exit(EXIT_FAILURE);
	}
	server_sockfd.events = POLLIN;

	if(bind(server_sockfd.fd,(struct sockaddr*)&sockaddr,sizeof(sockaddr)) == -1)
	{
		perror("ERROR: bind() failed.");
		exit(EXIT_FAILURE);
	}

	if(listen(server_sockfd.fd,MAXN_FD-1) == -1)
	{
		perror("ERROR: listen() failed.");
		exit(EXIT_FAILURE);
	}

	while(poll(&server_sockfd,1,-1) != -1)
	{
		int client_sockfd = accept(server_sockfd.fd,NULL,NULL);
		if(client_sockfd != -1)
		{
			pthread_mutex_lock(&stack.mutex);
			stack.fds[stack.nfds].fd = client_sockfd;
			stack.nfds++;
			pthread_mutex_unlock(&stack.mutex);
			pthread_cond_signal(&stack.cond);
		}
	}

	puts("poll() failed. (routine)");
	//exit(EXIT_FAILURE);
	return NULL;
}

bool init_server(list_t * _clients, list_t * _groups, size_t nsubroutines)
{
	pthread_t routine;
	pthread_t subroutines;

	clients = _clients;
	groups = _groups;

	for(size_t i = 0; i < MAXN_FD-1; i++)
	{
		stack.fds[i].events = POLLIN|POLLOUT;
	}

	if(pthread_create(&routine,NULL,server_routine,NULL) != 0)
	{
		return false;
	}

	for (size_t i = 0; i < nsubroutines; i++)
	{
		pthread_create(&subroutines,NULL,server_subroutine,NULL);
	}
	
	return true;
}