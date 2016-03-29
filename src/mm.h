#ifndef MM_ALLOCATOR_H_INCLUDED
#define MM_ALLOCATOR_H_INCLUDED

#include "common.h"

#define MM_MAX_ALLOCATOR 4

struct mm;

typedef size_t (*mm_allocator_size_t)(size_t size, size_t block);
typedef void(*mm_init_t)(void* allocator, size_t size, size_t block, size_t allocator_size, struct mm* mm, mm_err_t* err);
typedef void* (*mm_malloc_t)(void* allocator, void* buffer, size_t size, struct mm* mm, mm_err_t* err);
typedef void(*mm_free_t)(void* allocator, void* buffer, void* ptr, struct mm* mm, mm_err_t* err);
typedef void* (*mm_realloc_t)(void* allocator, void* buffer, void* ptr, struct mm* mm, mm_err_t* err);

typedef struct mm_allocator
{
	mm_init_t init;
	mm_malloc_t malloc;
	mm_free_t free;
	mm_realloc_t realloc;
	size_t size;
	size_t block_size;
	size_t alloc_size;
} mm_allocator_t;

typedef struct mm_type
{
	mm_allocator_size_t allocator_size;
	mm_init_t init;
	mm_malloc_t malloc;
	mm_free_t free;
	mm_realloc_t realloc;
} mm_type_t;

typedef struct mm
{
	void* buffer;
	uint8_t nallocators;
	uint8_t i;
	size_t header_blocks;
	mm_allocator_t allocs[MM_MAX_ALLOCATOR];
} mm_t;

void mm_create(mm_t* mm, mm_type_t* type, size_t size, size_t block, void* buffer, mm_err_t* err);
void mm_add_suballoc(mm_t* mm, mm_type_t* type, size_t block, mm_err_t* err);
void mm_done(mm_t* mm, mm_err_t* err);

void* mm_malloc(mm_t* mm, size_t size, mm_err_t* err);
void mm_free(mm_t* mm, void* ptr, mm_err_t* err);
void* mm_realloc(mm_t* mm, void* ptr, size_t size, mm_err_t* err);
void* mm_calloc(mm_t* mm, size_t size, mm_err_t* err);

void* mm_sub_malloc(mm_t* mm, size_t block, size_t size, mm_err_t* err);
void mm_sub_free(mm_t* mm, size_t block, void* ptr, mm_err_t* err);
void* mm_sub_realloc(mm_t* mm, size_t block, void* ptr, size_t size, mm_err_t* err);

#endif