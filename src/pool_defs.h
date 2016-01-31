#ifndef POOL_DEFS_H_INCLUDED
#define POOL_DEFS_H_INCLUDED

#ifdef __cplusplus
#define POOL_FUNC extern "C"
#else
#define POOL_FUNC
#endif

typedef unsigned char pool_u8;
typedef unsigned int pool_u32;

typedef pool_u8 pool_err;
typedef pool_u32 pool_size;


#ifndef POOL_MAX_SIZE
#define POOL_MAX_SIZE 4*1024*1024
#endif

#ifndef POOL_PAGE_SIZE
#define POOL_PAGE_SIZE 4*1024
#endif

#ifndef POOL_BLOCK_SIZE
#define POOL_BLOCK_SIZE 8
#endif

#define POOL_CEIL_DIV(a, b) (((a) + (b) - 1) / (b))

#define POOL_ERR_OK 0
#define POOL_ERR_OUT_OF_MEM 1
#define POOL_ERR_INVALID_POOL 2
#define POOL_ERR_INVALID_PTR 3

#define POOL_SET_ERR(err, val) if(err != NULL) *err = val
#define POOL_SET_ERR_IF(cond, err, val, ret) {if(err != NULL) {*err = val; return ret;}}

#endif