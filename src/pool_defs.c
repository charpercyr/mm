#include "pool_defs.h"

#if defined(_M_AMD64) || defined(__LP64__)

/** The lookup table for the log2 */
const static pool_u pool_log_table[64] =
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

/**
@fn pool_u pool_log2(pool_u n)
@brief Log 2 of an int (quick calculation)

@param n An integer

@return Log base 2 of n
*/
POOL_FUNC pool_u pool_log2(pool_u n)
{
	if (n == 0)
		return POOL_LOG_ERR;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;
	return pool_log_table[((pool_u)((n - (n >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}

#else

/** The lookup table for the log2 */
const static pool_u pool_log_table[32] =
{
	0,  9,  1, 10, 13, 21,  2, 29,
	11, 14, 16, 18, 22, 25,  3, 30,
	8, 12, 20, 28, 15, 17, 24,  7,
	19, 27, 23,  6, 26,  5,  4, 31
};

/**
@fn pool_u pool_log2(pool_u n)
@brief Log 2 of an int (quick calculation)

@param n An integer

@return Log base 2 of n
*/
POOL_FUNC pool_u pool_log2(pool_u n)
{
	if (n == 0)
		return POOL_LOG_ERR;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return pool_log_table[(pool_u)(n * 0x07C4ACDD) >> 27];
}

#endif

/**
	@fn pool_u pool_pow2(pool_u n)
	@brief Power of 2 of an int (quick calculation)

	@param n An integer

	@return 2^n
*/
POOL_FUNC pool_u pool_pow2(pool_u n)
{
	return 1 << n;
}
