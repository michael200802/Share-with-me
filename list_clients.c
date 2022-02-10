#include "list_clients.h"

bool comp_clients(const void * elem, const void * id)
{
    client_t * client = (client_t*)elem;
    const char * str1 = client->name, * str2 = (const char*)id;

    while(*str1 != '\0' || *str2 != '\0')
	{
		if(*str1 != *str2)
		{
			return false;
		}
		str1++;
		str2++;
	}
    return true;
}

void del_client(void * elem)
{
    client_t * client = (client_t*)elem;
    free(client->name);
    free(client->group);
    free(client);
}

list_operation_status_t add_client(list_t * list, const char * name, const char * group)
{
    client_t * client = (client_t*)malloc(sizeof(client_t));
    if(client == NULL)
    {
        return LIST_ERROR_MALLOC;
    }

    client->group = strdup(group);
    if(client->group == NULL)
    {
        free(client);
        return LIST_ERROR_MALLOC;
    }

    client->name = strdup(name); 
    if(client->name == NULL)
    {
        free(client->group);
        free(client);
        return LIST_ERROR_MALLOC;
    }

    return add_node(list,client);
}