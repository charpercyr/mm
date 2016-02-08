#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"

#ifndef _MSC_VER
#define fopen_s(file, filename, mode) *file=fopen(filename, mode)
#endif

void pool_buddy_dump(FILE* dump, pool_buddy* p);
void pool_buddy_stats_dump(FILE* dump, pool_buddy_stats* stats);
void pool_buddy_dump_all(FILE* dump, pool_buddy* p);

void pool_slab_dump(FILE* dump, pool_slab* p);
void pool_slab_stats_dump(FILE* dump, pool_slab_stats* stats);
void pool_slab_dump_all(FILE* dump, pool_slab* p);

void pool_mem_dump(FILE* dump, void* mem, pool_size size);

pool_t p;
char pool_mem[POOL_MAX_SIZE];

#define EXIT(err) exit(err)

#define N 2049

int print_list_elem(const pool_list_node* n, void* data)
{
	printf("%d ", (int)n->data);
	return 1;
}

int main()
{
	printf("Pool overhead : %d/%d (%f%%)\n", sizeof(pool_t), POOL_MAX_SIZE, 100.f*(float)sizeof(pool_t)/(float)POOL_MAX_SIZE);
	printf("Node size : %d\n", sizeof(pool_list_node));
	printf("Ptr size : %d\n", sizeof(void*));
	
	srand((unsigned int)time(NULL));
	pool_err err;
	pool_list l;
	pool_u i;
	FILE *before, *after;
	fopen_s(&before, "before.test", "w");
	fopen_s(&after, "after.test", "w");

	pool_init(&p, pool_mem, &err);
	if (err)
		EXIT(err);
	pool_list_init(&l, &p, NULL);
	for (i = 0; i < N; i++)
		pool_list_push_back(&l, i, NULL);

	pool_slab_dump_all(before, &p);
	fprintf(before, "\n======================= MEM =======================\n");
	pool_mem_dump(before, pool_mem, POOL_MAX_SIZE);

	pool_list_iterate(&l, print_list_elem, NULL, NULL);
	pool_list_delete(&l, NULL);

	pool_slab_dump_all(after, &p);
	fprintf(after, "\n======================= MEM =======================\n");
	pool_mem_dump(after, pool_mem, POOL_MAX_SIZE);

	printf("\n");
	EXIT(0);
}

void pool_buddy_dump(FILE* dump, pool_buddy* p)
{
	pool_u i, j;
	for (i = 0; i < POOL_BUDDY_TREE_SIZE;)
	{
		for (j = 0; j < 8 && i < POOL_PAGE_SIZE; i++, j++)
			fprintf(dump, "%.2x ", p->tree[i]);
		fprintf(dump, "\n");
	}
}

void pool_buddy_stats_dump(FILE* dump, pool_buddy_stats* stats)
{
	fprintf(dump, "size      %d\n", stats->size);
	fprintf(dump, "n blocks  %d\n", stats->n_blocks);
	fprintf(dump, "used      %d\n", stats->used);
	fprintf(dump, "n used b  %d\n", stats->n_blocks_used);
}

void pool_buddy_dump_all(FILE* dump, pool_buddy* p)
{
	pool_buddy_stats stats;
	pool_buddy_stat(p, &stats, NULL);
	fprintf(dump, "======================== BUDDY ========================\n");
	pool_buddy_dump(dump, p);
	fprintf(dump, "\n======================== STATS ========================\n");
	pool_buddy_stats_dump(dump, &stats);
}

void pool_slab_dump(FILE* dump, pool_slab* p)
{
	pool_u i, j;
	for (i = 0; i < POOL_SLAB_SLAB_SIZE;)
	{
		for (j = 0; j < 8 && i < POOL_SLAB_SLAB_SIZE; i++, j++)
			fprintf(dump, "%.2x", p->slabs[i]);
		fprintf(dump, "\n");
	}
}

void pool_slab_stats_dump(FILE* dump, pool_slab_stats* stats)
{
	fprintf(dump, "size      %d\n", stats->size);
	fprintf(dump, "used      %d\n", stats->used);
	fprintf(dump, "n_pages   %d\n", stats->n_pages);
	fprintf(dump, "n pages e %d\n", stats->n_pages_empty);
	fprintf(dump, "n pages p %d\n", stats->n_pages_partial);
	fprintf(dump, "n pages f %d\n", stats->n_pages_full);
	fprintf(dump, "n pages r %d\n", stats->n_pages_raw);
}

void pool_slab_dump_all(FILE* dump, pool_slab* p)
{
	pool_u i;
	pool_slab_stats stats;
	pool_slab_stat(p, &stats, NULL);
	fprintf(dump, "================================== SLABS ==================================\n");
	pool_slab_dump(dump, p);
	fprintf(dump, "\n================================== STATS ==================================\n");
	pool_slab_stats_dump(dump, &stats);
	fprintf(dump, "\n================================== BUDDIES ==================================\n");
	for (i = 0; i < POOL_SLAB_PAGE_N; i++)
	{
		fprintf(dump, "\n======================== BUDDY %d ========================\n", i);
		pool_buddy_dump_all(dump, p->buddies + i);
	}
}

void pool_mem_dump(FILE* dump, void* mem, pool_size size)
{
	pool_u i, j;
	for (i = 0; i < size;)
	{
		fprintf(dump, "%p ", (char*)mem + i);
		for (j = 0; j < 16 && i < size; i++, j++)
			fprintf(dump, "%.2x ", (unsigned int)((unsigned char*)mem)[i]);
		fprintf(dump, "\n");
	}
}