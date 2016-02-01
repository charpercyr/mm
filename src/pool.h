/** @file */
#ifndef POOL_H_INCLUDED
#define POOL_H_INCLUDED

#include "pool_slab.h"

#define pool_init pool_slab_init
#define pool_malloc pool_slab_malloc
#define pool_free pool_slab_free

typedef pool_slab pool;
typedef pool_slab_stats pool_stats;

#endif