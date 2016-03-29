#include "simple_mm.h"

static size_t simple_mm_allocator_size(size_t size, size_t block);
static void simple_mm_init(void* allocator, size_t size, mm_t* mm, mm_err_t* err);
static void* simple_mm_malloc(void* allocator, void* buffer, size_t size, mm_t* mm, mm_err_t* err);
static void simple_mm_free(void* allocator, void* buffer, void* ptr, mm_t* mm, mm_err_t* err);
static void* simple_mm_realoc(void* allocator, void* buffer, void* ptr, size_t size, mm_t* mm, mm_err_t* err);

mm_type_t _simple_mm_type = 
{
	.allocator_size = simple_mm_allocator_size,
	.init = simple_mm_init,
	.malloc = simple_mm_malloc,
	.free = simple_mm_free,
	.realloc = simple_mm_realoc
};
mm_type_t* simple_mm_type = &_simple_mm_type;