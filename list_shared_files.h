#pragma once

#include "list.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

typedef struct _shared_file
{
    char * path;
    char * name;
    size_t name_len;
    int fd;
    time_t last_change;
}shared_file_t;

shared_file_t * create_shared_file(const char * path);

bool comp_shared_files(const void * elem1, const void * id);

void del_shared_file(void * elem);

list_operation_status_t add_shared_file(list_t * list, const char * path);

#define declare_shared_files_list(name) list_t name = LIST_INITIALIZER(comp_shared_files,del_shared_file)