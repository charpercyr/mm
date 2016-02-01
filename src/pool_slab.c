#include "pool_slab.h"

/**
	@fn static pool_slab_page_type get_2_bits(char* buf, pool_u at)
	@brief Gets the type at the index

	@param buf The slab array
	@param at The index

	@return The type of page
*/
POOL_FUNC static pool_slab_page_type get_2_bits(char* buf, pool_u at)
{
	return (pool_slab_page_type)((POOL_GET_BIT(buf, 2 * at) << 1) | POOL_GET_BIT(buf, 2 * at + 1));
}

/**
	@fn static void set_2_bits(char* buf, pool_u at, pool_slab_page_type type)
	@brief Sets the type at the index

	@param buf the slab array
	@param at The index
	@param type The type to set
*/
POOL_FUNC static void set_2_bits(char* buf, pool_u at, pool_slab_page_type type)
{
	switch (type)
	{
	case EMPTY:
		POOL_UST_BIT(buf, 2 * at);
		POOL_UST_BIT(buf, 2 * at + 1);
		break;
	case PARTIAL:
		POOL_UST_BIT(buf, 2 * at);
		POOL_SET_BIT(buf, 2 * at + 1);
		break;
	case FULL:
		POOL_SET_BIT(buf, 2 * at);
		POOL_UST_BIT(buf, 2 * at + 1);
		break;
	case RAW:
		POOL_SET_BIT(buf, 2 * at);
		POOL_SET_BIT(buf, 2 * at + 1);
		break;
	}
}

/**
	@fn void pool_slab_init(pool_slab* p, void* mem, pool_err* err)
	@brief Initializes the slab pool

	@param[inout] p The slab struct
	@param[in] mem The memory base
	@param[out] err The error that happened
*/
POOL_FUNC void pool_slab_init(pool_slab* p, void* mem, pool_err* err)
{
	pool_u i;
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, );
	POOL_SET_ERR_IF(mem == NULL, err, POOL_ERR_INVALID_PTR, );
	p->mem = mem;
	for (i = 0; i < POOL_SLAB_SLAB_SIZE; i++)
		p->slabs[i] = 0x00;
	for (i = 0; i < POOL_SLAB_PAGE_N; i++)
	{
		pool_buddy_init(p->buddies + i, (char*)mem + i*POOL_SLAB_PAGE_SIZE, err);
		POOL_SET_ERR_IF(err ? *err : 0, err, *err, );
	}
}

POOL_FUNC static pool_u find_empty_page(pool_u8* slabs, pool_u n_pages)
{
	pool_u i, j;
	pool_u8 ok;
	for (i = 0; i < POOL_SLAB_PAGE_N - n_pages; i++)
	{
		ok = 1;
		for (j = i; j < n_pages; j++)
		{
			if (get_2_bits(slabs, j) != EMPTY)
			{
				ok = 0;
				break;
			}
		}
		if (ok)
			return i;
	}
	return POOL_SLAB_PAGE_N;
}

POOL_FUNC static pool_u find_page(pool_slab* p, pool_size size)
{
	pool_err err;
	pool_u i;
	for (i = 0; i < POOL_SLAB_PAGE_N; i++)
	{
		pool_slab_page_type type = get_2_bits(p->slabs, i);
		if (type == EMPTY || type == PARTIAL)
		{
			pool_u s = p->buddies[i].allocated;
			if (POOL_SLAB_PAGE_SIZE - s >= size)
				return i;
		}
	}
	return POOL_SLAB_PAGE_N;
}

/**
	@fn void* pool_slab_malloc(pool_slab* p, pool_size size, pool_err* err)
	@brief Allocates size bytes in the memory

	@param[in] p The slab struct
	@param[in] size The number of bytes to allocate
	@param[out] err The error that happened
*/
POOL_FUNC void* pool_slab_malloc(pool_slab* p, pool_size size, pool_err* err)
{
	POOL_SET_ERR(err, POOL_ERR_OK);
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, NULL);
	pool_u n_pages, first_page;
	pool_u i = 0;
	pool_size s;
	// RAW Page
	if (size > POOL_SLAB_PAGE_SIZE)
	{
		n_pages = POOL_CEIL_DIV(size + sizeof(pool_u), POOL_SLAB_PAGE_SIZE);
		first_page = find_empty_page(p->slabs, n_pages);
		POOL_SET_ERR_IF(first_page == POOL_SLAB_PAGE_N, err, POOL_ERR_OUT_OF_MEM, NULL);
		for (i = first_page; i < first_page + n_pages; i++)
			set_2_bits(p->slabs, i, RAW);
		void* ret = i*POOL_SLAB_PAGE_SIZE + (char*)p->mem;
		*((pool_u*)ret) = n_pages;
		((char*)ret) += sizeof(pool_u);
		return ret;
	}
	// Page with buddy allocator
	else
	{
		first_page = find_page(p, size);
		POOL_SET_ERR_IF(first_page == POOL_SLAB_PAGE_N, err, POOL_ERR_OUT_OF_MEM, NULL);
		void* ret = pool_buddy_malloc(p->buddies + first_page, size, err);
		POOL_SET_ERR_IF(err ? *err : 0, err, *err, );
		s = p->buddies[first_page].allocated*POOL_BUDDY_BLOCK_SIZE;
		POOL_SET_ERR_IF(err ? *err : 0, err, *err, );
		if (s == POOL_SLAB_PAGE_SIZE)
			set_2_bits(p->slabs, first_page, FULL);
		else
			set_2_bits(p->slabs, first_page, PARTIAL);
		return ret;
	}
}

/**
	@fn void pool_slab_free(pool_slab* p, void* ptr, pool_err* err)
	@brief Frees a previously allocated buffer

	@param[in] p The slab struct
	@param[in] ptr The buffer to free
	@param[out] err The error that happened

	@return The allocated buffer
*/
POOL_FUNC void pool_slab_free(pool_slab* p, void* ptr, pool_err* err)
{
	pool_u page;
	pool_slab_page_type type;
	pool_size s;
	pool_u i = 0;
	POOL_SET_ERR(err, POOL_ERR_OK);
	if (p == NULL)
		return;
	POOL_SET_ERR_IF(p == NULL, err, POOL_ERR_INVALID_POOL, err);
	POOL_SET_ERR_IF(ptr < p->mem || ptr > (char*)p->mem + POOL_SLAB_MAX_SIZE, err, POOL_ERR_INVALID_PTR, NULL);
	page = ((pool_u)ptr - (pool_u)p->mem) / POOL_SLAB_PAGE_SIZE;
	type = get_2_bits(p->slabs, page);
	POOL_SET_ERR_IF(type == EMPTY, err, POOL_ERR_INVALID_PTR, );
	if (type == PARTIAL || type == FULL)
	{
		pool_buddy_free(p->buddies + page, ptr, err);
		POOL_SET_ERR_IF(err ? *err : 0, err, *err, );
		s = p->buddies[page].allocated*POOL_BUDDY_BLOCK_SIZE;
		POOL_SET_ERR_IF(err ? *err : 0, err, *err, );
		if (s == 0)
			set_2_bits(p->slabs, page, EMPTY);
		else
			set_2_bits(p->slabs, page, PARTIAL);
	}
	else
	{
		POOL_SET_ERR_IF(ptr == (char*)p->mem + page*POOL_SLAB_PAGE_SIZE, err, POOL_ERR_INVALID_PTR, );
		s = *((pool_u*)ptr - 1);
		for (i = 0; i < s; i++)
			set_2_bits(p->slabs, i + page, EMPTY);
	}
}

/**
	@fn void pool_slab_stat(pool_slab* p, pool_slab_stats* stats, pool_err* err)
	@brief Stats the slab pool

	@param[in] p The slab struct
	@param[out] stats The stats
	@param[out] err The error that happened
*/
POOL_FUNC void pool_slab_stat(pool_slab* p, pool_slab_stats* stats, pool_err* err)
{
	pool_u i;
	pool_u s;
	POOL_SET_ERR(err, POOL_ERR_OK);
	stats->size = POOL_SLAB_PAGE_SIZE;
	stats->n_pages = POOL_SLAB_PAGE_N;
	stats->n_pages_empty = 0;
	stats->n_pages_full = 0;
	stats->n_pages_partial = 0;
	stats->n_pages_raw = 0;
	stats->used = 0;
	for (i = 0; i < POOL_SLAB_PAGE_N; i++)
	{
		pool_slab_page_type type = get_2_bits(p->slabs, i);
		switch (type)
		{
		case EMPTY:
			stats->n_pages_empty++;
			break;
		case PARTIAL:
			stats->n_pages_partial++;
			s = pool_buddy_size(p->buddies + i, err);
			POOL_SET_ERR_IF(err ? *err : 0, err, *err, );
			stats->used += s;
			break;
		case FULL:
			stats->n_pages_full++;
			stats->used += POOL_SLAB_PAGE_SIZE;
			break;
		case RAW:
			stats->n_pages_raw++;
			stats->used += POOL_SLAB_PAGE_SIZE;
			break;
		}
	}
}

/**
	@fn pool_u pool_slab_size(pool_slab* p, pool_err* err)
	@brief Calculates the number of allocated bytes

	@param[in] p The slab struct
	@param[out] err The error that happened

	@return The number of allocated bytes
*/
POOL_FUNC pool_u pool_slab_size(pool_slab* p, pool_err* err)
{
	pool_u i;
	pool_u s = 0;
	for (i = 0; i < POOL_SLAB_PAGE_N; i++)
	{
		pool_slab_page_type type = get_2_bits(p->slabs, i);
		switch (type)
		{
		case EMPTY:
			break;
		case PARTIAL:
			s += pool_buddy_size(p->buddies + i, err);
			POOL_SET_ERR_IF(err, err, *err, );
			break;
		case FULL:
			s += POOL_SLAB_PAGE_SIZE;
			break;
		case RAW:
			s += POOL_SLAB_PAGE_SIZE;
			break;
		}
	}
	return s;
}