/** @file */
#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include "pool.h"

#define POOL_LIST_ERR_INVALID_LIST 5
#define POOL_LIST_ERR_INVALID_POOL 6
#define POOL_LIST_ERR_INVALID_NODE 7
#define POOL_LIST_ERR_INVALID_FUNC 8

/**
	@defgroup LIST Doubly linked list
	@{
*/

/**
	@struct _pool_list_node
	@brief The linked list node
*/
typedef struct _pool_list_node
{
	/** The node's data */
	void* data;
	/** The previous node (NULL if none) */
	struct _pool_list_node* next;
	/** The next node (NULL if none) */
	struct _pool_list_node* prev;
} pool_list_node;

/**
	@struct _pool_list
	@brief A doubly linked list
*/
typedef struct _pool_list
{
	/** The pool used by the list */
	pool_t* pool;
	/** The head of the list */
	pool_list_node* head;
	/** The tail of the list */
	pool_list_node* tail;
} pool_list;

/** Callback function for iterate */
typedef pool_u8 (*pool_list_iterate_func)(const pool_list_node*, void*);

/**
	@fn void pool_list_init(pool_list* list, pool_t* pool, pool_err* err)
	@brief Initializes the list

	@param[inout] list The list
	@param[in] pool The memory pool
	@param[out] err The error
*/
POOL_FUNC void pool_list_init(pool_list* list, pool_t* pool, pool_err* err);

/**
	@fn void pool_list_add(pool_list* list, void* data, pool_list_node* after, pool_err* err)
	@brief Adds an item to the list

	@param[inout] list The list
	@param[in] data The data to add
	@param[in] after The node in the list that will be placed before the new node, NULL for the beginning of the list
	@param[out] err The error
*/
POOL_FUNC void pool_list_add(pool_list* list, void* data, pool_list_node* after, pool_err* err);

/**
	@fn void pool_list_push_back(pool_list* list, void* data, pool_err* err)
	@brief Adds an item to the end of the list

	@param[inout] list The list
	@param[in] data The data to add
	@param[out] err The error
*/
POOL_FUNC void pool_list_push_back(pool_list* list, void* data, pool_err* err);

/**
	@fn void pool_list_push_front(pool_list* list, void* data, pool_err* err)
	@brief Adds an item to the beginning of the list

	@param[inout] list The list
	@param[in] data The data to add
	@param[out] err The error
*/
POOL_FUNC void pool_list_push_front(pool_list* list, void* data, pool_err* err);

/**
	@fn void pool_list_remove(pool_list* list, pool_list_node* node, pool_err* err)
	@brief Removes an item from the list

	@param[inout] list The list
	@param[in] node The element to remove
	@param[out] err The error
*/
POOL_FUNC void pool_list_remove(pool_list* list, pool_list_node* node, pool_err* err);

/**
	@fn void pool_list_delete(pool_list* list, pool_err* err)
	@brief Removes all items from the list

	@param[inout] list The list
	@param[out] err The error
*/
POOL_FUNC void pool_list_delete(pool_list* list, pool_err* err);

/**
	@fn void pool_list_iterate(pool_list* list, pool_list_iterate_func func, void* data, pool_err* err)
	@brief Iterates over all the list's items

	@param[inout] list The list
	@param[in] func The callback function, the function should return 1 to continue and 0 to stop
	@param[in] data The data to pass to the function
	@param[out] err The error
*/
POOL_FUNC void pool_list_iterate(pool_list* list, pool_list_iterate_func func, void* data, pool_err* err);

/** @} */
#endif