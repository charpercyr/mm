/** @file */
#ifndef POOL_H_INCLUDED
#define POOL_H_INCLUDED

#include "pool_slab.h"

/**
@defgroup POOL Memory pool
@{
*/
/** pool_slab_init redefine */
#define pool_init pool_slab_init
/** pool_slab_malloc redefine */
#define pool_malloc pool_slab_malloc
/** pool_slab_free redefine */
#define pool_free pool_slab_free

/** Pool type */
typedef pool_slab pool_t;
/** Pool stats type */
typedef pool_slab_stats pool_stats_t;

/** @} */

#endif