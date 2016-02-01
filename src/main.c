#include <stdio.h>
#include <stdlib.h>
#include "pool.h"

typedef struct _node
{
	struct _node* next;
	struct _node* prev;
	int data;
} node;

typedef struct _list
{
	node* first;
	node* last;
	pool_slab* pool;
} list;

void list_init(list* l, pool_slab* pool);
void list_add(list* l, void* val);
void list_rem(list* l);
int list_get(list* l, pool_u i);
void list_delete(list* l);

void pool_buddy_dump(FILE* dump, pool_buddy* p);
void pool_buddy_stats_dump(FILE* dump, pool_buddy_stats* stats);
void pool_buddy_dump_all(FILE* dump, pool_buddy* p);

void pool_slab_dump(FILE* dump, pool_slab* p);
void pool_slab_stats_dump(FILE* dump, pool_slab_stats* stats);
void pool_slab_dump_all(FILE* dump, pool_slab* p);

void pool_mem_dump(FILE* dump, void* mem, pool_size size);

pool_slab p;
char pool_mem[POOL_MAX_SIZE];

#define EXIT(err) {system("pause"); return err;}

int getRand()
{
	return rand() << 16 | rand();
}

#define N 10000
int* tab[N];

void testMalloc()
{
	pool_u i;
	for (i = 0; i < N; i++)
		tab[i] = malloc(sizeof(int));
	for (i = 0; i < N; i++)
		free(tab[i]);
}

void testPool()
{
	pool_u i;
	for (i = 0; i < N; i++)
		tab[i] = pool_malloc(&p, sizeof(int), NULL);
	for (i = 0; i < N; i++)
		pool_free(&p, tab[i], NULL);
}

int main()
{
	pool_init(&p, pool_mem, NULL);
	//testMalloc();
	//testPool();
	printf("done\n");
	
	srand(time(NULL));
	pool_err err;
	list l;
	pool_u i;
	FILE *before, *after;
	fopen_s(&before, "before.test", "w");
	fopen_s(&after, "after.test", "w");

	pool_slab_init(&p, pool_mem, &err);
	if (err)
		EXIT(err);
	list_init(&l, &p);
	for (i = 0; i < N; i++)
		list_add(&l, getRand());

	pool_slab_dump_all(before, &p, POOL_PAGE_SIZE);

	printf("%d\n\n", sizeof(node));

	for (i = 0; i < N; i++)
		printf("%d ", list_get(&l, i));
	list_delete(&l);

	pool_slab_dump_all(after, &p, POOL_PAGE_SIZE);

	printf("\n");
	EXIT(0);
}

void list_init(list* l, pool_slab* pool)
{
	l->first = NULL;
	l->last = NULL;
	l->pool = pool;
}

void list_add(list* l,  int val)
{
	pool_err err;
	node* n = pool_slab_malloc(l->pool, sizeof(node), &err);
	if (err)
	{
		printf("%d\n", err);
		return;
	}
	n->next = NULL;
	n->prev = l->last;
	n->data = val;
	if (l->first == NULL)
	{
		l->first = n;
		l->last = n;
	}
	else
	{
		l->last->next = n;
		l->last = n;
	}
}

void list_rem(list* l)
{
	pool_err err;
	node* n = l->last;
	if (n != NULL)
	{
		if (l->first == l->last)
		{
			l->first = l->last = NULL;
		}
		else
			n->prev->next = NULL;
		pool_slab_free(l->pool, n, &err);
		if (err)
			printf("%d\n", err);
	}
}

int list_get(list* l, pool_u i)
{
	pool_u j;
	node* n = l->first;
	for (j = 0; j < i; j++)
	{
		n = n->next;
	}
	return n->data;
}

void list_delete(list* l)
{
	pool_err err;
	node* n = l->first;
	while (n != NULL)
	{
		node* n2 = n->next;
		pool_slab_free(l->pool, n, &err);
		if (err)
			printf("%d\n", err);
		n = n2;
	}
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