#ifndef MM_ALLOCATOR_H_INCLUDED
#define MM_ALLOCATOR_H_INCLUDED

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MM_MAX_ALLOCATOR 4

typedef size_t (*mm_allocator_size_t)(size_t size, size_t block);
typedef void(*mm_init_t)(void* allocator, size_t size, size_t block, size_t allocator_size, mm_err_t* err);
typedef void* (*mm_malloc_t)(void* allocator, void* buffer, size_t size, struct mm* mm, mm_err_t* err);
typedef void(*mm_free_t)(void* allocator, void* buffer, void* ptr, struct mm* mm, mm_err_t* err);
typedef void* (*mm_realloc_t)(void* allocator, void* buffer, void* ptr, struct mm* mm, mm_err_t* err);

typedef struct mm_type
{
	mm_allocator_size_t allocator_size;
	mm_init_t init;
	mm_malloc_t malloc;
	mm_free_t free;
	mm_realloc_t realloc;
} mm_type_t;

typedef struct mm_allocator_config
{
	mm_type_t* type;
	size_t block;
} mm_allocator_config_t;

typedef struct mm_config
{
	size_t size;
	size_t n_allocators;
	mm_allocator_config_t types[MM_MAX_ALLOCATOR];
} mm_config_t;

typedef struct mm_allocator
{
	size_t block;
	size_t allocator_size;
	mm_malloc_t malloc;
	mm_free_t free;
	mm_realloc_t realloc;
} mm_allocator_t;

typedef struct mm
{
	size_t size;
	size_t i;
	size_t n_allocators;
	void* allocs;
	void* mem;
	mm_allocator_t allocators[MM_MAX_ALLOCATOR];
} mm_t;

void mm_config_init(mm_config_t* config, size_t size, mm_type_t* type, size_t block, mm_err_t* err);
void mm_config_add_allocator(mm_config_t* config, mm_type_t* type, size_t block, mm_err_t* err);

void mm_init(mm_t* mm, mm_config_t* config, void* buffer, mm_err_t* err);

void* mm_malloc(mm_t* mm, size_t size, mm_err_t* err);
void mm_free(mm_t* mm, void* ptr, mm_err_t* err);
void* mm_realloc(mm_t* mm, void* ptr, size_t size, mm_err_t* err);

void* mm_sub_malloc(mm_t* mm, size_t size, size_t block, mm_err_t* err);
void mm_sub_free(mm_t* mm, void* ptr, size_t block, mm_err_t* err);
void* mm_realloc(mm_t* mm, void* ptr, size_t size, size_t block, mm_err_t* err);

#ifdef __cplusplus
}
#endif

#endif