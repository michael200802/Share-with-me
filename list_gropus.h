#pragma once

#include "list.h"

#include "list_shared_files.h"

#include <string.h>

typedef struct _group
{
    char * name;
    list_t shared_files;
}group_t;

bool comp_group(const void * elem, const void * id);

void del_group(void * elem);

list_operation_status_t add_group(list_t * list, const char * name);

#define declare_groups_list(name) list_t name = LIST_INITIALIZER(comp_group,del_group);