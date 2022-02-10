#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "list.h"

static inline char get_char()
{
	char option = getchar();
	while(getchar() != '\n');
	return option;
}

int main()
{
	pthread_t thread_server_routine;
	char buffer[100], buffer1[100], option = '\0';

	pthread_create(&thread_server_routine,NULL,server_routine,NULL);

	do
	{
		system("clear");

		if(option != '\0')
		{
			puts("Clients:");
			size_t i = 0;
			scroll_through_list(clients,ptrtonode,{i++; printf("%zu. %s\n",i,ptrtonode->str);});
			i = 0;
			puts("\nShared files:");
			scroll_through_list(shared_files,ptrtonode,{i++; printf("%zu. %s\n",i,ptrtonode->str);});
			putchar('\n');
		}

		puts("1. Add a new client.\n2. Remove client.\n3. Add new shared file.\n4. Remove new shared file.\n5. Exit.");

		option = get_char();

		system("clear");

		switch(option)
		{
			case '1'://add client
				puts("New client:");
				fgets(buffer1,100,stdin);sscanf(buffer1,"%s",buffer);
				switch(add_node(&clients,buffer))
				{
					case LIST_ERROR_NODE_FOUND:
						puts("Client already added.");
						while(getchar() != '\n');
						break;
					case LIST_ERROR_MALLOC:
						puts("Not enough heap.");
						while(getchar() != '\n');
						break;
					case LIST_SUCCESS:
						puts("Cliend added.");
						while(getchar() != '\n');
						break;
				}
				break;
			case '2'://rm client
				puts("Client to remove:");
				fgets(buffer1,100,stdin);sscanf(buffer1,"%s",buffer);
				switch(rm_node(&clients,buffer))
				{
					case LIST_ERROR_NODE_NOTFOUND:
						puts("Client non-existing.");
						while(getchar() != '\n');
						break;
					case LIST_SUCCESS:
						puts("Cliend removed.");
						while(getchar() != '\n');
						break;
				}
				break;
			case '3'://add file
				puts("New file to share:");
				fgets(buffer1,100,stdin);sscanf(buffer1,"%s",buffer);
				switch(add_node(&shared_files,buffer))
				{
					case LIST_ERROR_NODE_FOUND:
						puts("File already added.");
						while(getchar() != '\n');
						break;
					case LIST_ERROR_MALLOC:
						puts("Not enough heap.");
						while(getchar() != '\n');
						break;
					case LIST_SUCCESS:
						puts("File added.");
						while(getchar() != '\n');
						break;
				}
				break;
			case '4'://rm file
				puts("Remove shared file:");
				fgets(buffer1,100,stdin);sscanf(buffer1,"%s",buffer);
				switch(rm_node(&shared_files,buffer))
				{
					case LIST_ERROR_NODE_NOTFOUND:
						puts("File non-existing.");
						while(getchar() != '\n');
						break;
					case LIST_SUCCESS:
						puts("File removed.");
						while(getchar() != '\n');
						break;
				}
				break;
			case '5'://exit
				clean_list(&clients);
				clean_list(&shared_files);
				break;
			default:
				puts("Incorrect option");
		}
	}while(option != '5');
	return 0;
}
