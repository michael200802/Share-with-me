#include "list_groups.h"

bool comp_group(const void * elem, const void * id)
{
    const group_t * group = (const group_t*)elem;
    const char * str1 = group->name, * str2 = (const char *)id;

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

void del_group(void * elem)
{
    group_t * group = (group_t*)elem;
    free(group->name);
    clean_list(&group->shared_files);
    free(group);
}

inline list_operation_status_t add_group(list_t * list, const char * name)
{
    if(find_node(list,name) != NULL)
    {
        return LIST_ERROR_NODE_FOUND;
    }

    group_t * group = (group_t*)malloc(sizeof(group_t));
    if(group == NULL)
    {
        return LIST_ERROR_MALLOC;
    }
    declare_shared_files_list(shared_files_list);

    group->name = strdup(name);
    if(group->name == NULL)
    {
        free(group);
        return LIST_ERROR_MALLOC;
    }
    group->shared_files = shared_files_list;

    list_operation_status_t status = add_node(list,group);

    if(status != LIST_SUCCESS)
    {
        free(group->name);
        free(group);
    }
    return status;
}