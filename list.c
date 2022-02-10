#include "list.h"

inline list_operation_status_t add_node(list_t * list, void * elem)
{
	pthread_mutex_lock(&list->mutex);

	node_t * newnode = (node_t*)malloc(sizeof(node_t));
	if(newnode == NULL)
	{
		return LIST_ERROR_MALLOC;
	}
	newnode->elem = elem;
	newnode->nextnode = NULL;

	if(list->head == NULL)
	{
		list->head = newnode;
		list->tail = newnode;
		newnode->prevnode = NULL;
	}
	else
	{
		list->tail->nextnode = newnode;
		newnode->prevnode = list->tail;
		list->tail = newnode;
	}

	list->nnodes++;

	pthread_mutex_unlock(&list->mutex);

	return LIST_SUCCESS;
}

inline node_t * find_node(list_t list, const void * id)
{
	pthread_mutex_lock(&list.mutex);

	node_t * iterator = list.head;
	while(iterator != NULL)
	{
		if(list.comp_elem(iterator->elem,id) == true)
		{
			return iterator;
		}
		iterator = iterator->nextnode;
	}

	pthread_mutex_unlock(&list.mutex);
	return NULL;
}

inline list_operation_status_t rm_node(list_t * list, const void * elem)
{
	pthread_mutex_lock(&list->mutex);

	node_t * node = find_node(*list,elem);
	if(node == NULL)
	{
		return LIST_ERROR_NODE_NOTFOUND;
	}

	if(node->prevnode != NULL)
	{
		node->prevnode->nextnode = node->nextnode;
	}
	if(node->nextnode != NULL)
	{
		node->nextnode->prevnode = node->prevnode;
	}

	if(node == list->head)
	{
		list->head = node->nextnode;
	}
	if(node == list->tail)
	{
		list->tail = node->prevnode;
	}

	list->del_elem(node->elem);
	free(node);

	list->nnodes--;

	pthread_mutex_unlock(&list->mutex);

	return LIST_SUCCESS;
}

inline void clean_list(list_t * list)
{
	pthread_mutex_lock(&list->mutex);

	if(list->head != NULL)
	{
		node_t * iterator = list->head;
		while(iterator->nextnode != NULL)
		{
			iterator = iterator->nextnode;
			list->del_elem(iterator->prevnode->elem);
			free(iterator->prevnode);
		}
		list->del_elem(iterator->elem);
		free(iterator);
	}
	list->head = list->tail = NULL;
	list->nnodes = 0;

	pthread_mutex_unlock(&list->mutex);
}
