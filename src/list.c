#include "list.h"

void pool_list_init(pool_list* list, pool_t* pool, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	POOL_SET_ERR_IF(pool == NULL, err, POOL_LIST_ERR_INVALID_POOL, );

	list->pool = pool;
	list->head = NULL;
	list->tail = NULL;
}

void pool_list_add(pool_list* list, void* data, pool_list_node* after, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	POOL_SET_ERR_IF(list->pool == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	pool_err err2;
	pool_list_node* n = pool_malloc(list->pool, sizeof(pool_list_node), &err2);
	POOL_SET_ERR_IF(err2 != POOL_ERR_OK, err, err2, );
	n->data = data;
	if(list->head == NULL)
	{
		list->head = n;
		list->tail = n;
		n->next = NULL;
		n->prev = NULL;
	}
	else
	{
		if(after == NULL)
		{
			n->next = list->head;
			n->prev = NULL;
			list->head = n;
		}
		else
		{
			n->next = after->next;
			n->prev = after;
			after->next = n;
			if(n->next != NULL)
				n->next->prev = n;
		}
		if(after == list->tail)
			list->tail = n;
	}
}

void pool_list_push_back(pool_list* list, void* data, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	pool_list_add(list, data, list->tail, err);
}

void pool_list_push_front(pool_list* list, void* data, pool_err* err)
{
	pool_list_add(list, data, NULL, err);
}

void pool_list_remove(pool_list* list, pool_list_node* node, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	POOL_SET_ERR_IF(list->pool == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	POOL_SET_ERR_IF(node == NULL, err, POOL_LIST_ERR_INVALID_NODE, );

	if(node == list->head)
		list->head = node->next;
	if(node == list->tail)
		list->tail = node->prev;
	if(node->prev != NULL)
		node->prev->next = node->next;
	if(node->next != NULL)
		node->next->prev = node->prev;

	pool_free(list->pool, node, err);
}

void pool_list_delete(pool_list* list, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	while(list->head != NULL)
	{
		pool_list_remove(list, list->head, err);
		POOL_SET_ERR_IF(err != NULL ? *err : POOL_ERR_OK, err, *err, );
	}
}

void pool_list_iterate(pool_list* list, pool_list_iterate_func func, void* data, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	POOL_SET_ERR_IF(func == NULL, err, POOL_LIST_ERR_INVALID_FUNC, );
	pool_list_node* n = list->head;
	while(n != NULL){
		if(!func(n, data))
			break;
		n = n->next;
	}
}