#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct _node
{
    void * elem;
    struct _node * nextnode;
    struct _node * prevnode;
}node_t;

typedef struct
{
    node_t * head;
    node_t * tail;
    size_t nnodes;
	bool (*comp_elem)(const void *, const void *);
	void (*del_elem)(void *);
	pthread_mutex_t mutex;
}list_t;

enum list_operation_status{LIST_ERROR_NODE_FOUND , LIST_ERROR_NODE_NOTFOUND , LIST_ERROR_MALLOC , LIST_ERROR_CANNOT_OPEN_FILE , LIST_SUCCESS};
typedef enum list_operation_status list_operation_status_t;

#define LIST_INITIALIZER(lpfnComp, lpfnDel) {.head = NULL, .tail = NULL, .nnodes = 0, .mutex = PTHREAD_MUTEX_INITIALIZER, .comp_elem = lpfnComp, .del_elem = lpfnDel}

list_operation_status_t add_node(list_t * list, void * elem);

node_t * find_node(list_t * list, const void * id);

list_operation_status_t rm_node(list_t * list, const void * id);

void clean_list(list_t * list);

#define scroll_through_list(list, iterator_name, DO)															\
	{																											\
		pthread_mutex_lock(&list.mutex);																		\
		for(node_t * iterator_name = list.head;	iterator_name != NULL; iterator_name = iterator_name->nextnode)	\
		{																										\
			DO;																									\
		}																										\
		pthread_mutex_unlock(&list.mutex);																		\
	}
