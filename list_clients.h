#pragma once

#include "list.h"

typedef struct
{
    char * name;
    char * group;
}client_t;

bool comp_clients(const void * elem, const void * id);

void del_client(void * elem);

list_operation_status_t add_client(list_t * list, const char * name, const char * group);

#define declare_clients_list(name) list_t name = LIST_INITIALIZER(comp_clients,del_client)