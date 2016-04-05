#include "mm.h"

#define CHECK_TYPE(type, err, ret)\
	MM_SET_ERR_IF(type == NULL, err, MM_ERR_INVALID_TYPE, ret);\
	MM_SET_ERR_IF(type->init == NULL, err, MM_ERR_INVALID_TYPE, ret);\
	MM_SET_ERR_IF(type->allocator_size == NULL, err, MM_ERR_INVALID_TYPE, ret);\
	MM_SET_ERR_IF(type->malloc == NULL, err, MM_ERR_INVALID_TYPE, ret);\
	MM_SET_ERR_IF(type->free == NULL, err, MM_ERR_INVALID_TYPE, ret)

void mm_config_init(mm_config_t* config, size_t size, mm_type_t* type, size_t block, mm_err_t* err)
{
	MM_SET_ERR(err, MM_ERR_OK);
	MM_SET_ERR_IF(config == NULL, err, MM_ERR_INVALID_CONFIG, return;);
	MM_SET_ERR_IF(block == 0, err, MM_ERR_INVALID_SIZE, return;);
	MM_SET_ERR_IF(size < block || size % block != 0, err, MM_ERR_INVALID_SIZE, return;);
	CHECK_TYPE(type, err, );

	config->types[0].type = type;
	config->types[0].block = block;
	config->n_allocators = 1;
	config->size = size;
}

void mm_config_add_allocator(mm_config_t* config, mm_type_t* type, size_t block, mm_err_t* err)
{
	MM_SET_ERR(err, MM_ERR_OK);
	MM_SET_ERR_IF(config == NULL, err, MM_ERR_INVALID_CONFIG, return;);
	MM_SET_ERR_IF(config->n_allocators >= MM_MAX_ALLOCATOR, err, MM_ERR_TOO_MANY_ALLOCS, return;);
	MM_SET_ERR_IF(config->types[config->n_allocators].block == 0, err, MM_ERR_TOO_MANY_ALLOCS, return;);
	MM_SET_ERR_IF(config->types[config->n_allocators - 1].block % block, err, MM_ERR_INVALID_SIZE, return;);
	CHECK_TYPE(type, err, );

	config->types[config->n_allocators].type = type;
	config->types[config->n_allocators].block = block;
	config->n_allocators++;
}

void mm_init(mm_t* mm, mm_config_t* config, void* buffer, mm_err_t* err)
{
	MM_SET_ERR(err, MM_ERR_OK);
	MM_SET_ERR_IF(mm == NULL, err, MM_ERR_INVALID_MM, return;);
	MM_SET_ERR_IF(config == NULL, err, MM_ERR_INVALID_CONFIG, return;);
	MM_SET_ERR_IF(buffer == NULL, err, MM_ERR_INVALID_BUFFER, return;);

	size_t i, j;
	mm_err_t err2 = MM_ERR_OK;

	size_t size = config->size;
	size_t offset = 0;
	size_t mult = 1;

	mm->size = config->size;
	mm->i = 0;
	mm->n_allocators = config->n_allocators;

	for (i = 0; i < config->n_allocators; i++)
	{
		mm->allocators[i].block = config->types[i].block;
		mm->allocators[i].malloc = config->types[i].type->malloc;
		mm->allocators[i].free = config->types[i].type->free;
		mm->allocators[i].realloc = config->types[i].type->realloc;
		mm->allocators[i].allocator_size = config->types[i].type->allocator_size(size, config->types[i].block);
		size = config->types[i].block;
		offset += mult*mm->allocators[i].allocator_size;
		mult *= size / mm->allocators[i].block;
	}

	mm->allocs = buffer;
	mm->mem = (char*)buffer + offset;

	offset = 0;
	size = mm->size;
	for (i = 0; i < config->n_allocators; i++)
	{
		for (j = 0; j < mult; j++)
		{
			config->types[i].type->init((char*)buffer + offset, size, mm->allocators[i].block, mm->allocators[i].allocator_size, err);
			MM_SET_ERR_IF(err2, err, err2, return;);
			offset += mm->allocators[i].allocator_size;
		}
	}
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