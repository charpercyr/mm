#include <pool.h>
#include <stdio.h>

char* buffer[POOL_MAX_SIZE];

int main()
{
	pool_t pool;

	pool_init(&pool, buffer, NULL);

	void* b = pool_malloc(&pool, 1024, NULL);

	printf("0x%p", b);

	pool_free(&pool, b, NULL);

	return 0;
}