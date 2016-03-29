#include "common.h"
#include "config.h"

#ifdef MM_64_BITS
const static size_t mm_log_table[64] =
{
	63,  0, 58,  1, 59, 47, 53,  2,
	60, 39, 48, 27, 54, 33, 42,  3,
	61, 51, 37, 40, 49, 18, 28, 20,
	55, 30, 34, 11, 43, 14, 22,  4,
	62, 57, 46, 52, 38, 26, 32, 41,
	50, 36, 17, 19, 29, 10, 13, 21,
	56, 45, 25, 31, 35, 16,  9, 12,
	44, 24, 15,  8, 23,  7,  6,  5
};

size_t mm_log2(size_t n)
{
	if (n == 0)
		return (size_t)-1;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;
	return mm_log_table[((size_t)((n - (n >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}

#else

const static size_t mm_log_table[32] =
{
	0,  9,  1, 10, 13, 21,  2, 29,
	11, 14, 16, 18, 22, 25,  3, 30,
	8, 12, 20, 28, 15, 17, 24,  7,
	19, 27, 23,  6, 26,  5,  4, 31
};

size_t mm_log2(size_t n)
{
	if (n == 0)
		return (size_t)-1;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return mm_log_table[(size_t)(n * 0x07C4ACDD) >> 27];
}

#endif

size_t mm_exp2(size_t n)
{
	return 1ULL << n;
}

void* mm_memcpy(void* dest, const void* src, size_t size)
{
	if (dest == NULL || src == NULL || size == 0)
		return NULL;
	size_t* idest = dest;
	const size_t* isrc = src;
	size_t i = 0;
	for (i = 0; i < size / sizeof(size_t); i++)
	{
		*idest = *isrc;
		idest++;
		isrc++;
	}
	char* cdest = (char*)dest + i * sizeof(size_t);
	const char* csrc = (const char*)src + i * sizeof(size_t);
	for (i = 0; i < size - (size / sizeof(size_t))*sizeof(size_t); i++)
	{
		*cdest = *csrc;
		cdest++;
		csrc++;
	}
	return dest;
}