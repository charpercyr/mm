/** @file */
#ifndef BUDDY_H_INCLUDED
#define BUDDY_H_INCLUDED

#include "pool_defs.h"
/**
@defgroup BUDDY Buddy memory pool
@{
*/

/** Size of a buddy memory */
#define POOL_BUDDY_MAX_SIZE POOL_PAGE_SIZE
/** Size of a buddy block */
#define POOL_BUDDY_BLOCK_SIZE POOL_BLOCK_SIZE
/** Number of blocks in the memory */
#define POOL_BUDDY_BLOCK_N (POOL_PAGE_SIZE / POOL_BLOCK_SIZE)
/** Size of the buddy tree */
#define POOL_BUDDY_TREE_SIZE (POOL_CEIL_DIV(POOL_BUDDY_BLOCK_N, 4))

/**
	@struct _pool_buddy
	@brief The buddy pool header
*/
typedef struct _pool_buddy
{
	/** The buddy tree */
	pool_u8 tree[POOL_BUDDY_TREE_SIZE];
	/** The memory base */
	void* mem;
	/** The number of block allocated */
	pool_u allocated;
} pool_buddy;

/**
	@struct _pool_buddy_stats
	@brief Statistics about the buddy pool
*/
typedef struct _pool_buddy_stats
{
	/** The size of the buddy memory */
	pool_size size;
	/** The size of the used buddy memory */
	pool_size used;
	/** The number of blocks in the buddy memory */
	pool_u n_blocks;
	/** The number of used blocks in the buddy memory */
	pool_u n_blocks_used;
} pool_buddy_stats;

/**
	@fn void pool_buddy_init(pool_buddy* p, void* mem, pool_err* err)
	@brief Initializes the structure

	@param[out] p The buddy struct
	@param[in] mem The memory base
	@param[out] err The error that happened
*/
POOL_FUNC void pool_buddy_init(pool_buddy* p, void* mem, pool_err* err);

/**
	@fn void* pool_buddy_malloc(pool_buddy* p, pool_size size, pool_err* err)
	@brief Allocates size bytes in the memory

	@param[inout] p The buddy struct
	@param[in] size The number of bytes to allocate
	@param[out] err The error that happened

	@return The pointer to the allocated memory
*/
POOL_FUNC void* pool_buddy_malloc(pool_buddy* p, pool_size size, pool_err* err);

/**
	@fn void pool_buddy_free(pool_buddy* p, void* ptr, pool_err* err)
	@brief Frees a previously allocated buffer

	@param[inout] p The buddy struct
	@param[in] ptr The pointer to the buffer
	@param[out] err The error that happened
*/
POOL_FUNC void pool_buddy_free(pool_buddy* p, void* ptr, pool_err* err);

/**
	@fn void pool_buddy_stat(pool_buddy* p, pool_buddy_stats* stats, pool_err* err);
	@brief Stats the buddy mem

	@param[in] p The buddy struct
	@param[out] stats The statistics structure
	@param[out] err The error that happened
*/
POOL_FUNC void pool_buddy_stat(pool_buddy* p, pool_buddy_stats* stats, pool_err* err);

/**
	@fn static pool_u pool_buddy_size(pool_u8* tree, pool_u pos, pool_u8 depth)
	@brief Calculates the number of allocated bytes in the pool

	@param[in] tree The tree
	@param[in] pos The position in the tree we are looking at
	@param[in] depth The depth of the tree

	@return The number of allocated bytes
*/
POOL_FUNC pool_u pool_buddy_size(pool_buddy* p, pool_err* err);

/** @} */
#endif