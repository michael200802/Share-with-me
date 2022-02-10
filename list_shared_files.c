#include "list_shared_files.h"

inline shared_file_t * create_shared_file(const char * path)
{
    shared_file_t * file;
    struct stat file_status;

    file = (shared_file_t*)malloc(sizeof(shared_file_t));
    if(file == NULL)
    {
        return NULL;
    }

    file->path = strdup(path);
    if(file->path == NULL)
    {
        free(file);
        return NULL;
    }

    file->fd = open(file->path,O_RDONLY);
    if(file->fd == -1)
    {
        free(file->path);
        free(file);
        return NULL;
    }

    if(fstat(file->fd,&file_status) == -1)
    {
        close(file->fd);
        free(file->path);
        free(file);
        return NULL;
    }
    file->last_change = file_status.st_ctime;

    return file;
}

bool comp_shared_files(const void * elem1, const void * id)
{
    const shared_file_t * file1 = (shared_file_t*)elem1;
    const char * str1 = file1->path, * str2 = (const char *)id;

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

void del_shared_file(void * elem)
{
    shared_file_t * file = (shared_file_t*)elem;
    close(file->fd);
    free(file->path);
    free(file);
}

inline list_operation_status_t add_shared_file(list_t * list, const char * path)
{
    return add_node(list,create_shared_file(path));   
}
