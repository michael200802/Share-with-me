#include "list_gropus.h"

bool comp_str(const void * elem, const void * id)
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
    group_t * group = (group_t*)malloc(sizeof(group_t));
    declare_shared_file_list(shared_files_list);

    group->name = strdup(name);
    group->shared_files = shared_files_list;

    return add_node(list,group);
}