/** @file */
#ifndef POOL_DEFS_H_INCLUDED
#define POOL_DEFS_H_INCLUDED

#ifdef __cplusplus
#define POOL_FUNC extern "C"
#else
#define POOL_FUNC
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

/** 8 bits unsigned type */
typedef unsigned char pool_u8;

/** Error type */
typedef pool_u8 pool_err;
#if defined(_M_AMD64) || defined(__LP64__)
/** 64 bits unsigned type */
typedef unsigned long long int pool_u;
#else
/** 32 bits unsigned type */
typedef unsigned int pool_u;
#endif

/** Size type */
typedef pool_u pool_size;


/** Memory size (default 128K) */
#ifndef POOL_MAX_SIZE
#define POOL_MAX_SIZE (128*1024)
#endif

/** Page size (default 512B) */
#ifndef POOL_PAGE_SIZE
#define POOL_PAGE_SIZE 512
#endif

/** Block size (default 4B) */
#ifndef POOL_BLOCK_SIZE
#define POOL_BLOCK_SIZE 4
#endif

/** Divides and ceil the result */
#define POOL_CEIL_DIV(a, b) (((a) + (b) - 1) / (b))

/** 
	@defgroup ERRORS Error types
	@{
*/
/** No error */
#define POOL_ERR_OK 0
/** Out of memory */
#define POOL_ERR_OUT_OF_MEM 1
/** Invalid memory pool */
#define POOL_ERR_INVALID_POOL 2
/** Invalid pointer */
#define POOL_ERR_INVALID_PTR 3
/** Invalid size */
#define POOL_ERR_INVALID_SIZE 4
/** @} */

/**
@defgroup SET_ERR Error Setters
@{
*/
/** Sets the error if err is not null */
#define POOL_SET_ERR(err, val) if(err != NULL) *err = val
/** Sets the error if the condition is true and returns ret */
#define POOL_SET_ERR_IF(cond, err, val, ret) {if(cond) {POOL_SET_ERR(err, val); return ret;}}
/** @} */

/**
@defgroup BIT_GET_SET Bit getters and setters
@{
*/
/** Gets the bit n-th of a buffer */
#define POOL_GET_BIT(buf, pos) (((buf)[(pos)/8] & (1 << (7 - ((pos)%8)))) >> (7 - ((pos)%8)))

/** Sets the bit n-th of a buffer */
#define POOL_SET_BIT(buf, pos) ((buf)[(pos)/8] |= (1 << (7 - ((pos)%8))))
/** Unsets the bit n-th of a buffer */
#define POOL_UST_BIT(buf, pos) ((buf)[(pos)/8] &= ~(1 << (7 - ((pos)%8))))
/** @} */

/**
@defgroup EXP Exp and Log base 2
@{
*/
/** Log of 0 is MAX_INT */
#define POOL_LOG_ERR 0xffffffff

/**
	@fn pool_u pool_log2(pool_u n)
	@brief Log 2 of an int (quick calculation)

	@param n An integer

	@return log2(n)
*/
POOL_FUNC pool_u pool_log2(pool_u n);

/**
	@fn pool_u pool_pow2(pool_u n)
	@brief Power of 2 of an int (quick calculation)

	@param n An integer

	@return 2^n
*/
POOL_FUNC pool_u pool_pow2(pool_u n);
/** @} */

#endif