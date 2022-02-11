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
    if(find_node(list,name) != NULL)
    {
        return LIST_ERROR_NODE_FOUND;
    }
    
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

    list_operation_status_t status = add_node(list,client);
    if(status != LIST_SUCCESS)
    {
        free(client->name);
        free(client->group);
        free(client);
    }
    return status;
}