#include "server_backend.h"

typedef struct
{
	struct pollfd fd;
	list_t * sh_files;
}connection_t;

typedef struct
{
	connection_t connections[MAXN_FD-1];
	size_t nconnections;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}stack_t;

list_t * clients;
list_t * groups;
stack_t stack = {.nconnections = 0, .mutex = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER};

void * server_subroutine(void * arg)
{
	connection_t connection;
	struct pollfd client;
	list_t * sh_files;

	while(true)
	{
		//get a client
		pthread_mutex_lock(&stack.mutex);

		while(stack.nconnections == 0)
		{
			pthread_cond_wait(&stack.cond,&stack.mutex);
		}

		stack.nconnections--;
		connection = stack.connections[stack.nconnections];
		client = connection.fd;
		sh_files = connection.sh_files;

		pthread_mutex_unlock(&stack.mutex);

		//poll connection
		if(poll(&client,1,-1) != -1)
		{
			packet_type_t pack_type;

			if(client.revents == POLLERR || client.revents == 0)
			{
				close(client.fd);
				continue;
			}

			if(sh_files == NULL)
			{
				pack_type = PT_LOGIN;
			}
			else
			{
				if(recvpack(client.fd, &pack_type, sizeof(pack_type)) == false)
				{
					close(client.fd);
					continue;
				}
			}

			switch (pack_type)
			{
			case PT_LOGIN:
				{
					char * client_name;
					namesize_t client_name_len;
					const char * group_name;
					node_t * node;

					//get client's name

					if (recvpack(client.fd, &client_name_len, sizeof(client_name_len)) == false || client_name_len == 0)
					{
						close(client.fd);
						continue;
					}

					client_name = (char*)malloc(client_name_len);
					if(client_name == NULL)
					{
						close(client.fd);
						continue;
					}

					if (recvpack(client.fd, client_name, client_name_len) == false)
					{
						free(client_name);
						close(client.fd);
						continue;
					}

					//find the client
					node = find_node(clients, client_name);

					if(node != NULL)
					{
						//set up the new connection

						group_name = ((client_t*)node->elem)->group;

						node = find_node(groups, group_name);

						sh_files = &((group_t*)node->elem)->shared_files;

						client.events = POLLOUT | POLLIN;

						pthread_mutex_lock(&stack.mutex);
						stack.connections[stack.nconnections].fd = client;
						stack.connections[stack.nconnections].sh_files = sh_files;
						stack.nconnections++;
						pthread_mutex_unlock(&stack.mutex);
						pthread_cond_signal(&stack.cond);

						//printf("%s/%d logged in.\n",client_name, client.fd);
					}
					else
					{
						//close the connection
						close(client.fd);
					}

					free(client_name);
				}
				break;
			case PT_REQUEST_FOR_UPDATE:
					//send every file
					//printf("%d is requesting update: ",client.fd);
					{
						bool error_with_connection = false;
						psize_t nfiles = sh_files->nnodes;

						if(sendpack(client.fd, &nfiles, sizeof(nfiles)) == false)
						{
							close(client.fd);
							continue;
						}

						scroll_through_list
						(
							(*sh_files),
							ptrtonode,
							{
								shared_file_t * file;
								struct stat file_stat;
								size_t file_size;
								char * mapped_file;
								packetsize_t packsize = {};

								file = ((shared_file_t*)ptrtonode->elem);

								if(fstat(file->fd,&file_stat) == -1 || file_stat.st_ctime == file->last_change)
								{
									if(sendpack(client.fd, &packsize, sizeof(packsize)) == false)
									{
										error_with_connection = true;
										close(client.fd);
										break;
									}
									//puts("Already updated.");
									continue;
								}
								file->last_change = file_stat.st_ctime;

								file_size = file_stat.st_size;

								mapped_file = mmap(NULL,file_size,PROT_READ,MAP_PRIVATE,file->fd,0);
								if(mapped_file == (char *)-1)
								{
									if(sendpack(client.fd, &packsize, sizeof(packsize)) == false)
									{
										error_with_connection = true;
										close(client.fd);
										break;
									}
									continue;
								}

								packsize.name_len = file->name_len+1;
								packsize.file_size = file_size;

								if(sendpack(client.fd, &packsize, sizeof(packsize)) == false)
								{
									error_with_connection = true;
									close(client.fd);
									munmap(mapped_file,file_size);
									break;
								}

								if(sendpack(client.fd, file->name, packsize.name_len) == false)
								{
									error_with_connection = true;
									close(client.fd);
									munmap(mapped_file,file_size);
									break;
								}

								if(sendpack(client.fd, mapped_file, file_size) == false)
								{
									error_with_connection = true;
									close(client.fd);
									munmap(mapped_file,file_size);
									break;
								}

								munmap(mapped_file,file_size);

								//puts("Update completed.");
							}
						);

						if(error_with_connection == false)
						{
							pthread_mutex_lock(&stack.mutex);
							stack.connections[stack.nconnections] = connection;
							stack.nconnections++;
							pthread_mutex_unlock(&stack.mutex);
							pthread_cond_signal(&stack.cond);
						}
					}
				break;

			case PT_LOGOUT:
				close(client.fd);
				break;

			default:
				close(client.fd);
				break;
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
	sockaddr.sin_port = htons(PORT);
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	for(size_t i = 0; i < 8; i++)
	{
		sockaddr.sin_zero[i] = 0;
	}

	server_sockfd.fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_sockfd.fd == -1)
	{
		perror("socket() failed.");
		exit(EXIT_FAILURE);
	}
	server_sockfd.events = POLLIN;

	if(bind(server_sockfd.fd,(struct sockaddr*)&sockaddr,sizeof(sockaddr)) == -1)
	{
		perror("bind() failed.");
		exit(EXIT_FAILURE);
	}

	if(listen(server_sockfd.fd,MAXN_FD-1) == -1)
	{
		perror("listen() failed.");
		exit(EXIT_FAILURE);
	}

	while(poll(&server_sockfd,1,-1) != -1)
	{
		int client_sockfd = accept(server_sockfd.fd,NULL,NULL);
		if(client_sockfd != -1)
		{
			pthread_mutex_lock(&stack.mutex);
			stack.connections[stack.nconnections].fd.fd = client_sockfd;
			stack.connections[stack.nconnections].fd.events = POLLIN;
			stack.connections[stack.nconnections].sh_files = NULL;
			stack.nconnections++;
			pthread_mutex_unlock(&stack.mutex);
			pthread_cond_signal(&stack.cond);
		}
	}

	puts("poll() failed. (routine)");
	exit(EXIT_FAILURE);
	return NULL;
}

bool init_server(list_t * _clients, list_t * _groups, size_t nsubroutines)
{
	pthread_t routine;
	pthread_t subroutines;

	clients = _clients;
	groups = _groups;

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