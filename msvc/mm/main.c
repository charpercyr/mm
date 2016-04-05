#include <mm.h>
#include <simple_mm.h>

#define MEM_SIZE (128*1024*1024)
#define BLOCK_SIZE_1 (8*1024)
#define BLOCK_SIZE_2 8

char* mem[MEM_SIZE];

int main()
{
	mm_config_t config;
	mm_config_init(&config, MEM_SIZE, simple_mm_type, BLOCK_SIZE_1, NULL);
	mm_config_add_allocator(&config, simple_mm_type, BLOCK_SIZE_2, NULL);
	mm_t mm;
	mm_init(&mm, &config, mem, NULL);
	return 0;
}