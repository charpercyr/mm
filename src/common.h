#ifndef MM_COMMON_H_INCLUDED
#define MM_COMMON_H_INCLUDED

#include <stdint.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef uint8_t mm_err_t;

#define MM_ERR_OK				0
#define MM_ERR_INVALID_CONFIG	1
#define MM_ERR_INVALID_MM		2
#define MM_ERR_INVALID_TYPE		3
#define MM_ERR_INVALID_SIZE		4
#define MM_ERR_INVALID_BUFFER	5
#define MM_ERR_TOO_MANY_ALLOCS	6
#define MM_ERR_INIT_ERR			7

#define MM_SET_ERR(err, val) if(err) *err = val
#define MM_SET_ERR_IF(cond, err, val, action) if(cond){ MM_SET_ERR(err, val); action;}

#define MM_DIV(a, b) ((a) / (b))
#define MM_CEIL_DIV(a, b) (((a) + (b) - 1) / (b))

size_t mm_log2(size_t n);
size_t mm_exp2(size_t n);

void* mm_memcpy(void* dest, const void* src, size_t size);

#endif