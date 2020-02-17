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

struct am_alloc;

typedef
void *am_alloc_fn(
        void *ptr,
        size_t oldsz,
        size_t newsz,
        struct am_alloc *self);

struct am_alloc {
    am_alloc_fn *fn;
};

AM_ATTR_NON_NULL((1)) AM_ATTR_MALLOC
static AM_INLINE void *
am_malloc(struct am_alloc *alloc, size_t sz)
{
    return alloc->fn(NULL, 0, sz, alloc);
}

AM_ATTR_NON_NULL((1))
static AM_INLINE void
am_free(struct am_alloc *alloc, void *ptr, size_t sz)
{
    (void)alloc->fn(ptr, sz, 0, alloc);
}

AM_ATTR_NON_NULL((1)) AM_ATTR_MALLOC AM_ATTR_WARN_UNUSED_RESULT
static AM_INLINE void *
am_realloc(struct am_alloc *alloc, void *ptr, size_t oldsz, size_t newsz)
{
    return alloc->fn(ptr, oldsz, newsz, alloc);
}

/** @brief Initialize a malloc-based allocator
 * This allocator doesn't require any additional space
 */
AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_init_default(struct am_alloc *alloc);

struct am_alloc_stack {
    struct am_alloc alloc;
    struct obstack obstack;
};

/** @brief Initialize a stack allocator
 * @param sz Initial allocation, in bytes
 * @note This allocator does not support arbitrary frees
 */
AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_init_stack(struct am_alloc_stack *alloc);
AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_destroy_stack(struct am_alloc_stack *alloc);


#endif /* ifndef AM_ALLOC_H */
