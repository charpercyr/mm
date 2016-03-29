#include "mm.h"

void mm_create(mm_t* mm, mm_type_t* type, size_t size, size_t block, void* buffer, mm_err_t* err)
{
	MM_SET_ERR(err, MM_ERR_OK);
	MM_SET_ERR_IF(mm == NULL, err, MM_ERR_INVALID_MM, return);
	MM_SET_ERR_IF(type == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(type->init == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(type->malloc == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(type->free == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(size == 0 || block == 0, err, MM_ERR_INVALID_SIZE, return);
	MM_SET_ERR_IF(size % block != 0, err, MM_ERR_INVALID_SIZE, return);
	MM_SET_ERR_IF(buffer == NULL, err, MM_ERR_INVALID_BUFFER, return);

	mm->buffer = buffer;
	mm->nallocators = 1;
	mm->i = 1;
	mm->header_blocks = 0;

	mm->allocs[0].size = size;
	mm->allocs[0].block_size = block;
	mm->allocs[0].alloc_size = type->allocator_size(size, block);
	mm->allocs[0].init = type->init;
	mm->allocs[0].malloc = type->malloc;
	mm->allocs[0].free = type->free;
	mm->allocs[0].realloc = type->realloc;
}

void mm_add_suballoc(mm_t* mm, mm_type_t* type, size_t block, mm_err_t* err)
{
	MM_SET_ERR(err, MM_ERR_OK);
	MM_SET_ERR_IF(mm == NULL, err, MM_ERR_INVALID_MM, return);
	MM_SET_ERR_IF(type == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(type->init == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(type->malloc == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(type->free == NULL, err, MM_ERR_INVALID_TYPE, return);
	MM_SET_ERR_IF(mm->i != mm->nallocators, err, MM_ERR_ALREADY_DONE, return);
	MM_SET_ERR_IF(mm->nallocators >= MM_MAX_ALLOCATOR, err, MM_ERR_TOO_MANY_ALLOCS, return);
	MM_SET_ERR_IF(mm->allocs[mm->nallocators - 1].block_size % block != 0, err, MM_ERR_INVALID_SIZE, return);

	mm->allocs[mm->nallocators].size = mm->allocs[mm->nallocators - 1].block_size;
	mm->allocs[mm->nallocators].block_size = block;
	mm->allocs[mm->nallocators].init = type->init;
	mm->allocs[mm->nallocators].malloc = type->malloc;
	mm->allocs[mm->nallocators].free = type->free;
	mm->allocs[mm->nallocators].realloc = type->realloc;
}

void mm_done(mm_t* mm, mm_err_t* err)
{
	MM_SET_ERR(err, MM_ERR_OK);
	MM_SET_ERR_IF(mm == NULL, err, MM_ERR_INVALID_MM, return);
	MM_SET_ERR_IF(mm->i != mm->nallocators, err, MM_ERR_ALREADY_DONE, return);

	mm->i = 0;
	size_t mult = 1;
	size_t i;
	size_t header_size = 0;
	for (i = 0; i < mm->nallocators; i++)
	{
		header_size += mult * mm->allocs[i].alloc_size;
		mult *= mm->allocs[i].size / mm->allocs[i].block_size;
	}
	mm->header_blocks = MM_CEIL_DIV(header_size, mm->allocs[0].block_size);

	mult = 1;
	size_t offset = 0;
	for (i = 0; i < mm->nallocators; i++)
	{
		size_t j;
		for (j = 0; j < mult; j++)
		{
			mm_err_t err2;
			mm->allocs[i].init((char*)mm->buffer + offset, mm->allocs[i].size, mm->allocs[i].block_size, mm->allocs[i].alloc_size, mm, &err2);
			MM_SET_ERR_IF(err2, err, err2, return);
			offset += mult * mm->allocs[i].alloc_size;
			mult *= mm->allocs[i].size / mm->allocs[i].block_size;
		}
	}
	mm->buffer = (char*)mm->buffer + offset;
}

void* mm_malloc(mm_t* mm, size_t size, mm_err_t* err)
{

}

void mm_free(mm_t* mm, void* ptr, mm_err_t* err)
{

}

void* mm_realloc(mm_t* mm, void* ptr, size_t size, mm_err_t* err)
{

}

void* mm_calloc(mm_t* mm, size_t size, mm_err_t* err)
{

}

void* mm_sub_malloc(mm_t* mm, size_t block, size_t size, mm_err_t* err)
{

}

void mm_sub_free(mm_t* mm, size_t block, void* ptr, mm_err_t* err)
{

}
void* mm_sub_realloc(mm_t* mm, size_t block, void* ptr, size_t size, mm_err_t* err)
{

}
