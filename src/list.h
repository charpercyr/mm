#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include "pool.h"

#define POOL_LIST_ERR_INVALID_LIST 5
#define POOL_LIST_ERR_INVALID_POOL 6
#define POOL_LIST_ERR_INVALID_NODE 7
#define POOL_LIST_ERR_INVALID_FUNC 8

typedef struct _pool_list_node
{
	void* data;
	struct _pool_list_node* next;
	struct _pool_list_node* prev;
} pool_list_node;

typedef struct _pool_list
{
	pool_t* pool;
	pool_list_node* head;
	pool_list_node* tail;
} pool_list;

typedef pool_u8 (*pool_list_iterate_func)(const pool_list_node*, void*);

void pool_list_init(pool_list* list, pool_t* pool, pool_err* err);
void pool_list_add(pool_list* list, void* data, pool_list_node* after, pool_err* err);
void pool_list_push_back(pool_list* list, void* data, pool_err* err);
void pool_list_push_front(pool_list* list, void* data, pool_err* err);
void pool_list_remove(pool_list* list, pool_list_node* node, pool_err* err);
void pool_list_delete(pool_list* list, pool_err* err);
void pool_list_iterate(pool_list* list, pool_list_iterate_func func, void* data, pool_err* err);

#endif