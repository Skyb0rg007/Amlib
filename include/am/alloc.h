/** @file am/alloc.h
 * @brief Definitions of different memory allocators and a usable interface
 */

#ifndef AM_ALLOC_H
#define AM_ALLOC_H 1

#include <stdlib.h>
#include "am/common.h"

/** @brief Type of an allocator function */
typedef void *am_alloc_fn(void *ptr, size_t oldsize, size_t newsize, void *ud);

/** @brief Type of a relocate function */
typedef void am_relocate_fn(void *data, intptr_t oldloc, void *ud);

struct am_alloc {
    am_alloc_fn *fn;
    void *ud;
};

struct am_relocate {
    am_relocate_fn *fn;
    void *ud;
};

/** @brief Relocate */
#define am_relocate(reloc, ptr, oldloc) ((reloc)->fn ? (reloc)->fn(ptr, (intptr_t)oldloc, (reloc)->ud) : (void)0)

/** @def am_malloc
 * @param alloc The allocator
 * @param sz The size of the memory to be allocated
 * Acts like `malloc`
 */
#define am_malloc(alloc, sz) ((alloc)->fn(NULL, 0, sz, (void *)(alloc)->ud))
/** @def am_free
 * @param alloc The allocator
 * @param ptr The pointer to free
 * @param sz The size of the memory to be allocated
 * Acts like `free`
 */
#define am_free(alloc, ptr, sz) ((void)(alloc)->fn(ptr, sz, 0, (void *)(alloc)->ud))
/** @def am_realloc
 * @param alloc The allocator
 * @param ptr The pointer to reallocate
 * @param oldsz The old size of the memory
 * @param newsz The new size of the memory
 * Acts like `realloc`
 */
#define am_realloc(alloc, ptr, oldsz, newsz) ((alloc)->fn(ptr, oldsz, newsz, (void *)(alloc)->ud))

/** @brief Allocator function based on the standard allocator malloc */
static
void *am_alloc_fn_standard(void *ptr, size_t oldsz, size_t newsz, void *ud)
{
    (void)oldsz;
    (void)ud;
    if (newsz == 0) {
        free(ptr);
        return NULL;
    } else {
        return realloc(ptr, newsz);
    }
}

/** @brief Allocator based on the standard allocator malloc */
static AM_ATTR_UNUSED
const struct am_alloc am_alloc_standard = { am_alloc_fn_standard, NULL };

#endif /* ifndef AM_ALLOC_H */
