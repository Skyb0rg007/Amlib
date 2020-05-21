/** @file am/alloc.h
 * @brief Definition of an allocator interface
 *
 * Usage:
 *
 * Define an allocator 
 */

#ifndef AM_ALLOC_H
#define AM_ALLOC_H 1

#include <stddef.h>
#include <obstack.h>
#include "am/macros.h"
#include "am/data/list.h"
#include "am/data/vector.h"

/* struct am_alloc; */

typedef void *am_alloc_fn(void *ptr, size_t oldsz, size_t newsz, void *userdata);

/* am_malloc(am_alloc_fn *f, void *ud, size_t sz) */
#define am_malloc(f, ud, sz) (f(NULL, 0, sz, ud))
/* am_free(am_alloc_fn *f, void *ud, void *ptr, size_t sz) */
#define am_free(f, ud, ptr, sz) (f(ptr, sz, 0, ud))
/* am_realloc(am_alloc_fn *f, void *ud, void *ptr, size_t oldsz, size_t newsz) */
#define am_realloc(f, ud, ptr, oldsz, newsz) (f(ptr, oldsz, newsz, ud))

/** @brief Realloc-based allocator
 * Note that this allocator doesn't investigate its userdata argument
 */
static AM_INLINE void *am_alloc_standard(void *ptr, size_t oldsz, size_t newsz, void *ud)
{
    (void)oldsz; (void)ud;
    if (newsz == 0) {
        free(ptr);
        return NULL;
    } else {
        return realloc(ptr, newsz);
    }
}

/* struct am_alloc_stack { */
    /* struct am_alloc alloc; */
    /* struct obstack obstack; */
/* }; */

/** @brief Initialize a stack allocator
 * @param sz Initial allocation, in bytes
 * @note This allocator does not support arbitrary frees or realloc
 * Use this allocator when working with memory allocated and freed as a stack
 */
/* AM_PUBLIC AM_ATTR_NON_NULL((1)) */
/* void am_alloc_init_stack(struct am_alloc_stack *alloc); */
/* AM_PUBLIC AM_ATTR_NON_NULL((1)) */
/* void am_alloc_destroy_stack(struct am_alloc_stack *alloc); */

/* struct am_alloc_pool { */
    /* struct am_alloc alloc; */
    /* am_vector(struct am_alloc_pool_page *) pages; */
    /* union am_alloc_pool_node *stack; */
    /* size_t chunk_size; */
/* }; */

/** @brief Initialize a pool allocator
 * @param chunks_per_block Number of chunks to allocate in one larger allocation
 * @note This allocator must be used with data of the same size
 * Use this allocator when all allocation are of the same size
 */
/* AM_PUBLIC AM_ATTR_NON_NULL((1)) */
/* void am_alloc_init_pool(struct am_alloc_pool *alloc, size_t chunk_size); */
/* AM_PUBLIC AM_ATTR_NON_NULL((1)) */
/* void am_alloc_destroy_pool(struct am_alloc_pool *alloc); */

#endif /* ifndef AM_ALLOC_H */
