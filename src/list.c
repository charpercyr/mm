#include "list.h"

/**
	@fn void pool_list_init(pool_list* list, pool_t* pool, pool_err* err)
	@brief Initializes the list

	@param[inout] list The list
	@param[in] pool The memory pool
	@param[out] err The error
*/
POOL_FUNC void pool_list_init(pool_list* list, pool_t* pool, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	POOL_SET_ERR_IF(pool == NULL, err, POOL_LIST_ERR_INVALID_POOL, );

	list->pool = pool;
	list->head = NULL;
	list->tail = NULL;
}

/**
	@fn void pool_list_add(pool_list* list, void* data, pool_list_node* after, pool_err* err)
	@brief Adds an item to the list

	@param[inout] list The list
	@param[in] data The data to add
	@param[in] after The node in the list that will be placed before the new node, NULL for the beginning of the list
	@param[out] err The error
*/
POOL_FUNC void pool_list_add(pool_list* list, void* data, pool_list_node* after, pool_err* err)
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

/**
	@fn void pool_list_push_back(pool_list* list, void* data, pool_err* err)
	@brief Adds an item to the end of the list

	@param[inout] list The list
	@param[in] data The data to add
	@param[out] err The error
*/
POOL_FUNC void pool_list_push_back(pool_list* list, void* data, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	pool_list_add(list, data, list->tail, err);
}

/**
	@fn void pool_list_push_front(pool_list* list, void* data, pool_err* err)
	@brief Adds an item to the beginning of the list

	@param[inout] list The list
	@param[in] data The data to add
	@param[out] err The error
*/
POOL_FUNC void pool_list_push_front(pool_list* list, void* data, pool_err* err)
{
	pool_list_add(list, data, NULL, err);
}

/**
	@fn void pool_list_remove(pool_list* list, pool_list_node* node, pool_err* err)
	@brief Removes an item from the list

	@param[inout] list The list
	@param[in] node The element to remove
	@param[out] err The error
*/
POOL_FUNC void pool_list_remove(pool_list* list, pool_list_node* node, pool_err* err)
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

/**
	@fn void pool_list_delete(pool_list* list, pool_err* err)
	@brief Removes all items from the list

	@param[inout] list The list
	@param[out] err The error
*/
POOL_FUNC void pool_list_delete(pool_list* list, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(list == NULL, err, POOL_LIST_ERR_INVALID_LIST, );
	while(list->head != NULL)
	{
		pool_list_remove(list, list->head, err);
		POOL_SET_ERR_IF(err != NULL ? *err : POOL_ERR_OK, err, *err, );
	}
}

/**
	@fn void pool_list_iterate(pool_list* list, pool_list_iterate_func func, void* data, pool_err* err)
	@brief Iterates over all the list's items

	@param[inout] list The list
	@param[in] func The callback function, the function should return 1 to continue and 0 to stop
	@param[in] data The data to pass to the function
	@param[out] err The error
*/
POOL_FUNC void pool_list_iterate(pool_list* list, pool_list_iterate_func func, void* data, pool_err* err)
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