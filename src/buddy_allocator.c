#include "buddy_allocator.h"

static size_t buddy_allocator_size(size_t size, size_t block)
{
	return MM_CEIL_DIV(MM_DIV(size, block), 4);
}

static void buddy_init(void* allocator, mm_allocator_info_t* info, mm_err_t* err)
{
	char* tree = allocator;
	size_t i = 0;
	for (i = 0; i < info->allocator_size; i++)
		tree[i] = 0x00;
	tree[0] = 0x80;
}

static void* buddy_malloc(mm_t* mm, void* allocator, void* base, size_t size, mm_allocator_info_t* info, mm_err_t* err)
{
	return NULL;
}

static void buddy_free(mm_t* mm, void* allocator, void* base, void* ptr, struct mm_allocator_info* info, mm_err_t* err)
{

}

static void* buddy_realloc(mm_t* mm, void* allocator, void* base, void* ptr, size_t size, mm_allocator_info_t* info, mm_err_t* err)
{
	return NULL;
}

static mm_type_t _mm_buddy_allocator_type =
{
	.allocator_size = buddy_allocator_size,
	.init = buddy_init,
	.malloc = buddy_malloc,
	.free = buddy_free,
	.realloc = buddy_realloc
};

mm_type_t* mm_buddy_allocator_type = &_mm_buddy_allocator_type;