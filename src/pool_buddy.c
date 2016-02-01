#include "pool_buddy.h"


/**
	@fn void pool_buddy_init(pool_buddy* p, void* mem, pool_err* err)
	@brief Initializes the structure

	@param[out] p The buddy struct
	@param[in] mem The memory base
	@param[out] err The error that happened
*/
POOL_FUNC void pool_buddy_init(pool_buddy* p, void* mem, pool_err* err)
{
	pool_u i;
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, );
	POOL_SET_ERR_IF(mem == NULL, err, POOL_ERR_INVALID_PTR, );
	p->mem = mem;
	p->allocated = 0;

	for (i = 0; i < POOL_BUDDY_TREE_SIZE; i++)
		p->tree[i] = 0xff;
	p->tree[0] = 0x7f;
}

/**
	@fn static pool_u check_pos(pool_u8* tree, pool_u pos, pool_u8 depth)
	@brief Checks if a memory slot (at pos) is free

	@param[in] tree The buddy tree
	@param[in] pos The memory slot
	@param[in] depth The depth of the tree

	@return 1 If available, 0 if not
*/
POOL_FUNC static pool_u check_pos(pool_u8* tree, pool_u pos, pool_u8 depth)
{
	pool_u pos_level = pool_log2(pos);
	if (pos_level > depth)
		return 1;
	if (POOL_GET_BIT(tree, pos) == 0)
		return 0;
	if (check_pos(tree, 2 * pos, depth) == 0)
		return 0;
	if (check_pos(tree, 2 * pos + 1, depth) == 0)
		return 0;
	return 1;
}


/**
	@fn static pool_u find_pos(pool_u8* tree, pool_u8 max_level, pool_u pos, pool_u8 depth)
	@brief Finds an available memory slot
	
	@param[in] tree The buddy tree
	@param[in] max_level The level in the tree to allocate into
	@param[in] pos The position in the tree we are looking at
	@param[in] depth The depth of the tree

	@return The position in the tree of an available memory slot, 0 if none
*/
POOL_FUNC static pool_u find_pos(pool_u8* tree, pool_u8 max_level, pool_u pos, pool_u8 depth)
{
	pool_u level = pool_log2(pos);
	pool_u res = 0;
	if (level > max_level)
		return 0;
	if (level == max_level)
	{
		if (POOL_GET_BIT(tree, pos))
			return pos;
		else
			return 0;
	}
	else
	{
		if (POOL_GET_BIT(tree, pos))
		{
			res = find_pos(tree, max_level, 2 * pos, depth);
			if (res)
				return res;
			res = find_pos(tree, max_level, pos * 2 + 1, depth);
			if (res)
				return res;
		}
		return 0;
	}
}

/**
	@fn void* pool_buddy_malloc(pool_buddy* p, pool_size size, pool_err* err)
	@brief Allocates size bytes in the memory

	@param[inout] p The buddy struct
	@param[in] size The number of bytes to allocate
	@param[out] err The error that happened

	@return The pointer to the allocated memory
*/
POOL_FUNC void* pool_buddy_malloc(pool_buddy* p, pool_size size, pool_err* err)
{
	pool_u max_level;
	pool_u pos;
	pool_u pos_level;
	pool_u8 depth = pool_log2(POOL_CEIL_DIV(POOL_BUDDY_MAX_SIZE, POOL_BUDDY_BLOCK_SIZE));
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, NULL);
	POOL_SET_ERR_IF(size == 0, err, POOL_ERR_INVALID_SIZE, NULL);
	POOL_SET_ERR_IF(size > POOL_BUDDY_MAX_SIZE, err, POOL_ERR_OUT_OF_MEM, NULL);

	if (size < POOL_BUDDY_BLOCK_SIZE)
		size = POOL_BUDDY_BLOCK_SIZE;

	max_level = pool_log2(POOL_CEIL_DIV(POOL_BUDDY_MAX_SIZE, size));
	pos = find_pos(p->tree, max_level, 1, depth);
	POOL_SET_ERR_IF(pos == 0, err, POOL_ERR_OUT_OF_MEM, NULL);

	POOL_UST_BIT(p->tree, pos);

	pos_level = pool_log2(pos);

	for (; pos_level < depth; pos_level++)
		pos *= 2;

	p->allocated += pool_pow2(depth - max_level);

	return (void*)((pos - POOL_BUDDY_BLOCK_N)*POOL_BUDDY_BLOCK_SIZE + (char*)p->mem);
}

/**
	@fn void pool_buddy_free(pool_buddy* p, void* ptr, pool_err* err)
	@brief Frees a previously allocated buffer

	@param[inout] p The buddy struct
	@param[in] ptr The pointer to the buffer
	@param[out] err The error that happened
*/
POOL_FUNC void pool_buddy_free(pool_buddy* p, void* ptr, pool_err* err)
{
	pool_u ptr_pos;
	pool_u test_pos;
	pool_u8 depth = pool_log2(POOL_CEIL_DIV(POOL_BUDDY_MAX_SIZE, POOL_BUDDY_BLOCK_SIZE));
	POOL_SET_ERR(err, POOL_ERR_OK);
	if (ptr == NULL)
		return;
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, );
	POOL_SET_ERR_IF(ptr < p->mem || (char*)ptr > (char*)p->mem + POOL_BUDDY_MAX_SIZE, err, POOL_ERR_INVALID_PTR, );

	ptr_pos = (pool_u)(POOL_CEIL_DIV((char*)ptr - (char*)p->mem, POOL_BUDDY_BLOCK_SIZE) + POOL_BUDDY_BLOCK_N);
	while (POOL_GET_BIT(p->tree, ptr_pos) && ptr_pos != 0)
		ptr_pos /= 2;
	POOL_SET_ERR_IF(ptr_pos == 0, err, POOL_ERR_INVALID_PTR, );
	test_pos = ptr_pos;
	for (; pool_log2(test_pos) < depth; test_pos *= 2);
	void* test_addr = (void*)((test_pos - POOL_BUDDY_BLOCK_N)*POOL_BUDDY_BLOCK_SIZE + (char*)p->mem);
	POOL_SET_ERR_IF(test_addr != ptr, err, POOL_ERR_INVALID_PTR, );
	POOL_SET_BIT(p->tree, ptr_pos);

	p->allocated -= pool_pow2(depth - pool_log2(ptr_pos));
}

/**
	@fn static pool_u calc_size(pool_u8* tree, pool_u pos, pool_u8 depth)
	@brief Calculates the number of used blocks in the pool

	@param[in] tree The tree
	@param[in] pos The position in the tree we are looking at
	@param[in] depth The depth of the tree

	@return The number of used blocks
*/
POOL_FUNC static pool_u calc_size(pool_u8* tree, pool_u pos, pool_u8 depth)
{
	pool_u8 pos_level = pool_log2(pos);
	if (pos_level > depth)
		return 0;
	if (!POOL_GET_BIT(tree, pos))
		return pool_pow2(depth - pos_level);
	else
		return calc_size(tree, 2 * pos, depth) + calc_size(tree, 2 * pos + 1, depth);
}

/**
	@fn void pool_buddy_stat(pool_buddy* p, pool_buddy_stats* stats, pool_err* err);
	@brief Stats the buddy mem

	@param[in] p The buddy struct
	@param[out] stats The statistics structure
	@param[out] err The error that happened
*/
POOL_FUNC void pool_buddy_stat(pool_buddy* p, pool_buddy_stats* stats, pool_err* err)
{
	pool_u8 depth = pool_log2(POOL_CEIL_DIV(POOL_BUDDY_MAX_SIZE, POOL_BUDDY_BLOCK_SIZE));
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, );
	POOL_SET_ERR_IF(stats == NULL, err, POOL_ERR_INVALID_PTR, );
	stats->size = POOL_BUDDY_MAX_SIZE;
	stats->n_blocks = POOL_BUDDY_BLOCK_N;
	stats->n_blocks_used = calc_size(p->tree, 1, depth);
	stats->used = stats->n_blocks_used*POOL_BUDDY_BLOCK_SIZE;
}

/**
@fn static pool_u pool_buddy_size(pool_u8* tree, pool_u pos, pool_u8 depth)
@brief Calculates the number of allocated bytes in the pool

@param[in] tree The tree
@param[in] pos The position in the tree we are looking at
@param[in] depth The depth of the tree

@return The number of allocated bytes
*/
POOL_FUNC pool_u pool_buddy_size(pool_buddy* p, pool_err* err)
{
	pool_u8 depth = pool_log2(POOL_CEIL_DIV(POOL_BUDDY_MAX_SIZE, POOL_BUDDY_BLOCK_SIZE));
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, 0);
	return POOL_BUDDY_BLOCK_SIZE*calc_size(p->tree, 1, depth);
}