#ifndef BUDDY_H_INCLUDED
#define BUDDY_H_INCLUDED

#include "pool_defs.h"

#define POOL_BUDDY_MAX_SIZE POOL_PAGE_SIZE
#define POOL_BUDDY_BLOCK_N (POOL_PAGE_SIZE / POOL_BLOCK_SIZE)
#define POOL_TREE_SIZE (POOL_CEIL_DIV(POOL_BUDDY_BLOCK_N, 4))

typedef struct _pool_buddy
{
	pool_u8 tree[POOL_TREE_SIZE];
	void* mem;
} pool_buddy;

typedef struct _pool_buddy_stats
{
	pool_u32 size;
	pool_u32 used;
	pool_u32 n_blocks;
	pool_u32 n_blocks_used;
} pool_buddy_stats;

POOL_FUNC void pool_buddy_init(pool_buddy* p, pool_err* err);

POOL_FUNC void* pool_buddy_malloc(pool_buddy* p, pool_size size, pool_err* err);
POOL_FUNC void pool_buddy_free(pool_buddy* p, void* ptr, pool_err* err);

#endif