/** @file */
#ifndef POOL_SLAB_H_INCLUDED
#define POOL_SLAB_H_INCLUDED

#include "pool_buddy.h"

/**
@defgroup SLAB Slab memory pool
@{
*/

/** Size of the slab mem */
#define POOL_SLAB_MAX_SIZE POOL_MAX_SIZE
/** Size of a slab page */
#define POOL_SLAB_PAGE_SIZE POOL_PAGE_SIZE
/** Number of pages in mem */
#define POOL_SLAB_PAGE_N (POOL_CEIL_DIV(POOL_SLAB_MAX_SIZE, POOL_SLAB_PAGE_SIZE))
/** Size of the slab array */
#define POOL_SLAB_SLAB_SIZE (POOL_CEIL_DIV(POOL_SLAB_PAGE_N, 4))

/**
	@struct _pool_slab
	@brief The slab pool header
*/
typedef struct _pool_slab
{
	/** The slab array */
	pool_u8 slabs[POOL_SLAB_SLAB_SIZE];
	/** The array of buddy pool (1 per page) */
	pool_buddy buddies[POOL_SLAB_PAGE_N];
	/** Base of memory */
	void* mem;
} pool_slab;

/**
	@struct _pool_slab_stats
	@brief Statistics about the slab pool
*/
typedef struct _pool_slab_stats
{
	/** Size of the pool */
	pool_size size;
	/** Used bytes in the pool */
	pool_size used;
	/** Number of pages in the pool */
	pool_u n_pages;
	/** Number of empty pages in the pool */
	pool_u n_pages_empty;
	/** Number of partial pages in the pool */
	pool_u n_pages_partial;
	/** Number of full pages in the pool */
	pool_u n_pages_full;
	/** Number of raw pages in the pool */
	pool_u n_pages_raw;
} pool_slab_stats;

/**
	@enum _pool_slab_page_type
	@brief The types of pages
*/
typedef enum _pool_slab_page_type
{
	/** No allocation in the page */
	EMPTY = 0,
	/** Some allocation in the page but not full */
	PARTIAL = 1,
	/** Page is fully allocated*/
	FULL = 2,
	/** Allocation for buffer with its size greater than the page size */
	RAW = 3
} pool_slab_page_type;

/**
	@fn void pool_slab_init(pool_slab* p, void* mem, pool_err* err)
	@brief Initializes the slab pool

	@param[inout] p The slab struct
	@param[in] mem The memory base
	@param[out] err The error that happened
*/
POOL_FUNC void pool_slab_init(pool_slab* p, void* mem, pool_err* err);

/**
	@fn void* pool_slab_malloc(pool_slab* p, pool_size size, pool_err* err)
	@brief Allocates size bytes in the memory

	@param[in] p The slab struct
	@param[in] size The number of bytes to allocate
	@param[out] err The error that happened
*/
POOL_FUNC void* pool_slab_malloc(pool_slab* p, pool_size size, pool_err* err);

/**
	@fn void pool_slab_free(pool_slab* p, void* ptr, pool_err* err)
	@brief Frees a previously allocated buffer

	@param[in] p The slab struct
	@param[in] ptr The buffer to free
	@param[out] err The error that happened

	@return The allocated buffer
*/
POOL_FUNC void pool_slab_free(pool_slab* p, void* ptr, pool_err* err);

/**
	@fn void pool_slab_stat(pool_slab* p, pool_slab_stats* stats, pool_err* err)
	@brief Stats the slab pool

	@param[in] p The slab struct
	@param[out] stats The stats
	@param[out] err The error that happened
*/
POOL_FUNC void pool_slab_stat(pool_slab* p, pool_slab_stats* stats, pool_err* err);

/**
	@fn pool_u pool_slab_size(pool_slab* p, pool_err* err)
	@brief Calculates the number of allocated bytes

	@param[in] p The slab struct
	@param[out] err The error that happened

	@return The number of allocated bytes
*/
POOL_FUNC pool_u pool_slab_size(pool_slab* p, pool_err* err);

/** @} */

#endif