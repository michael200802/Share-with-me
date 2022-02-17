#include <stdio.h>
#include <stdint.h>
#include "list_clients.h"
#include "list_groups.h"
#include "server_backend.h"

static inline char get_char()
{
	char option = getchar();
	if(option != '\n')
	{
		while(getchar() != '\n');
	}
	return option;
}

void get_str(char * buffer)
{
	char buffer1[100];
	fgets(buffer1,100,stdin);
	sscanf(buffer1,"%s",buffer);
}

int main()
{
	declare_clients_list(clients);
	declare_groups_list(groups);
	char buffer[100], buffer1[100], option = '\0';
	node_t * node;

	init_server(&clients,&groups,MAXN_FD);

	do
	{
		system("clear");

		if(option != '\0')
		{
			puts("Clients:");
			size_t i = 0;
			scroll_through_list
			(
				clients,
				ptrtonode,
				{
					client_t * client = (client_t*)(ptrtonode->elem); 

					i++;
					printf("%zu. %s ---> %s\n",i,client->name,client->group);
				}
			);

			i = 0;
			puts("\nGroups:");
			scroll_through_list
			(
				groups,
				ptrtonode,
				{	
					group_t * group = (group_t*)(ptrtonode->elem);
					size_t j = 0;

					i++;
					printf("%zu. %s\n",i,group->name);

					puts("\tShared files:");
					scroll_through_list
					(
						group->shared_files,
						node_sh_files,
						{
							shared_file_t * sh_file = (shared_file_t*)(node_sh_files->elem);

							j++;
							printf("\t%zu. %s\n",j,sh_file->path);
						}
					);
				}
			);
			putchar('\n');
		}

		puts
		(
			"1. Add a new client.\n"
			"2. Remove client.\n"
			"3. Add new group.\n"
			"4. Remove group.\n"
			"5. Add file to group's shared file list:\n"
			"6. Remove file from group's shared file list:\n"
			"7. Exit."
		);

		option = get_char();

		system("clear");

		switch(option)
		{
			case '1'://add client
				puts("New client:");
				printf("\tName: ");
				get_str(buffer);
				printf("\tGroup: ");
				get_str(buffer1);
				if(find_node(&groups,buffer1) == NULL)
				{
					puts("ERROR: Non-existent group.");
					get_char();
					continue;
				}
				switch(add_client(&clients,buffer,buffer1))
				{
					case LIST_ERROR_NODE_FOUND:
						puts("ERROR: Client already added.");
						break;
					case LIST_ERROR_MALLOC:
						puts("ERROR: Not enough heap.");
						break;
					case LIST_SUCCESS:
						puts("Cliend added.");
						break;
				}
				get_char();
				break;
			case '2'://rm client
				printf("Client to remove: ");
				get_str(buffer);
				switch(rm_node(&clients,buffer))
				{
					case LIST_ERROR_NODE_NOTFOUND:
						puts("ERROR: Client non-existing.");
						break;
					case LIST_SUCCESS:
						puts("Cliend removed.");
						break;
				}
				get_char();
				break;
			case '3'://add group
				printf("New group: ");
				get_str(buffer);
				switch(add_group(&groups,buffer))
				{
					case LIST_ERROR_NODE_FOUND:
						puts("ERROR: Group already added.");
						break;
					case LIST_ERROR_MALLOC:
						puts("ERROR: Not enough heap.");
						break;
					case LIST_SUCCESS:
						puts("Group added.");
						break;
				}
				get_char();
				break;
			case '4'://rm group
				printf("Group to remove: ");
				get_str(buffer);
				switch(rm_node(&groups,buffer))
				{
					case LIST_ERROR_NODE_NOTFOUND:
						puts("ERROR: Non-existent group.");
						break;
					case LIST_SUCCESS:
						puts("Group removed.");
						break;
				}
				get_char();
				break;
			case '5'://add file
				printf("Group: ");
				get_str(buffer);
				node = find_node(&groups,buffer);
				if(node == NULL)
				{
					puts("ERROR: Non-existent group.");
					get_char();
					continue;
				}
				printf("File path: ");
				get_str(buffer1);
				switch(add_shared_file(&((group_t*)node->elem)->shared_files,buffer1))
				{
					case LIST_ERROR_NODE_FOUND:
						puts("ERROR: File already added.");
						break;
					case LIST_ERROR_CANNOT_OPEN_FILE:
						puts("ERROR: Couldn't open the file.");
						break;
					case LIST_ERROR_MALLOC:
						puts("ERROR: Not enough heap.");
						break;
					case LIST_SUCCESS:
						puts("File added.");
						break;
				}
				get_char();
				break;
			case '6'://rm file
				printf("Group: ");
				get_str(buffer);
				node = find_node(&groups,buffer);
				if(node == NULL)
				{
					puts("ERROR: Non-existent group.");
					get_char();
					continue;
				}
				printf("File: ");
				get_str(buffer1);
				switch(rm_node( &((group_t*)node->elem)->shared_files, buffer1))
				{
				case LIST_ERROR_NODE_NOTFOUND:
						puts("ERROR: Non-existent file.");
						break;
				case LIST_SUCCESS:
						puts("File removed.");
						break;
				}
				get_char();
				break;
			case '7'://exit
				clean_list(&clients);
				clean_list(&groups);
				break;
			default:
				puts("Incorrect option");
				get_char();
		}
	}while(option != '7');
	return 0;
}
