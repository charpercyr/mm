#include <mm.h>
#include <stdio.h>
#include <buddy_allocator.h>

#define ALLOC_SIZE (128*1024)
#define MEM_SIZE (8*1024*1024)
#define BLOCK_SIZE_1 (16*1024)
#define BLOCK_SIZE_2 8

char* mem[MEM_SIZE];
char alloc_mem[ALLOC_SIZE];

#ifdef _WIN64
#define ADDR_STRING "%.16x "
#else
#define ADDR_STRING "%.8x "
#endif

void dump(mm_t* mm, FILE* out)
{
	size_t offset = 0;
	size_t i, j, k;
	size_t mult = 1;
	size_t size = mm->size;

	size_t allocator_size = mm->alloc_size;

	fprintf(out, "ALLOCATOR SIZE : %d (0x%x)\n\n", allocator_size, allocator_size);
	for (i = 0; i < mm->n_allocators; i++)
	{
		for (j = 0; j < mult; j++)
		{
			fprintf(out, "ALLOC %d %d\n", i, j);
			fprintf(out, ADDR_STRING, offset);
			for (k = 0; k < mm->allocators[i].allocator_size; k++)
			{
				fprintf(out, "%.2x", ((unsigned char*)mm->allocs)[offset + k]);
				if ((k + 1) % 0x10 == 0)
				{
					fprintf(out, "\n");
					if (k < mm->allocators[i].allocator_size - 1)
						fprintf(out, ADDR_STRING, offset + k + 1);
				}
				else if ((k + 1) % 0x02 == 0)
					fprintf(out, " ");
			}
			offset += mm->allocators[i].allocator_size;
			fprintf(out, "\n");
		}
		fprintf(out, "=======================================\n\n");
		mult *= size / mm->allocators[i].block;
		size = mm->allocators[i].block;
	}

	size_t memory_size = mm->size;
	offset = 0;

	fprintf(out, "MEMORY SIZE : %d (0x%x)\n\n", memory_size, memory_size);

	fprintf(out, ADDR_STRING, offset);
	for (i = 0; i < memory_size; i++)
	{
		fprintf(out, "%.2x", ((unsigned char*)mm->allocs)[offset + i]);
		if ((i + 1) % 0x10 == 0)
		{
			fprintf(out, "\n");
			if (i < memory_size - 1)
				fprintf(out, ADDR_STRING, offset + i + 1);
		}
		else if ((i + 1) % 0x02 == 0)
			fprintf(out, " ");
	}
}

int main()
{
	mm_config_t config;
	mm_config_init(&config, MEM_SIZE, mm_buddy_allocator_type, BLOCK_SIZE_1, NULL);
	mm_config_add_allocator(&config, mm_buddy_allocator_type, BLOCK_SIZE_2, NULL);

	mm_t mm;
	mm_init(&mm, &config, alloc_mem, mem, NULL);

	
	FILE** out;
	fopen_s(&out, "dump.txt", "w");
	
	dump(&mm, out);
	return 0;
}