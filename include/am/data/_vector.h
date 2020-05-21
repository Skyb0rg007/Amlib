
/*
 * Implementation of a type-generic vector
 * Container members are relocated, so only use with elements that can
 * be memcpy'd.
 */

#ifndef AM_DATA_VECTOR_H
#define AM_DATA_VECTOR_H 1

#include <stddef.h> /* NULL */
#include <stdlib.h> /* malloc, realloc */
#include <stdint.h> /* uint32_t */
#include <stdbool.h>
#include <string.h> /* memmove */
#include "am/macros.h"
#include "am/logging.h"

/* am_vector(T) */

/* Initialization */
/* #define AM_VECTOR_INITIALIZER */
/* void am_vector_init(am_vector(T) *v); */
/* void am_vector_free(am_vector(T) *v); */

/* Optimizations */
/* bool am_vector_reserve(am_vector(T) *v, uint32_t size); */
/* bool am_vector_shrink_to_fit(am_vector(T) *v); */

/* Query */
/* uint32_t am_vector_capacity(am_vector(T) *v); */
/* uint32_t am_vector_size(am_vector(T)); */
/* T *am_vector_data(am_vector(T)); */
/* T& am_vector_at(am_vector(T) *v, uint32_t index); */

/* Insertion */
/* bool am_vector_insert(am_vector(T) *v, uint32_t index, T value); */
/* bool am_vector_push_back(am_vector(T) *v, T value); */
/* T *am_vector_emplace(am_vector(T) *v, uint32_t index); */
/* T *am_vector_emplace_back(am_vector(T) *v); */

/* Iteration */
/* T *am_vector_begin(am_vector(T) *v); */
/* T *am_vector_end(am_vector(T) *v); */
/* am_vector_foreach(T *it, am_vector(T) *v) { ... } */

/*****************************************************************************
 *
 * Implementation
 *
 *****************************************************************************/

#define am_vector(T) \
    struct { uint32_t _size, _capacity; T *_data; }

#define AM_VECTOR_INITIALIZER { 0, 0, NULL }

#define am_vector_init(v)                                             \
    ((v) != NULL                                                      \
     ? (void)((v)->_size = 0, (v)->_capacity = 0, (v)->_data = NULL) \
     : am_critical("am_vector_init"))
#define am_vector_free(v)                                       \
    ((v) != NULL                                                \
     ? free((v)->_data)                                        \
     : am_critical("am_vector_free"))
#define am_vector_reserve(v, n)    am__vector_reserve(am__vector_cast(v), n)
#define am_vector_shrink_to_fit(v) am__vector_shrink_to_fit(am__vector_cast(v))
#define am_vector_at(v, i)         ((v)->_data[i])
#define am_vector_capacity(v)      ((uint32_t)(v)->_capacity)
#define am_vector_size(v)          ((uint32_t)(v)->_size)
#define am_vector_data(v)          ((v)->_data)
#define am_vector_insert(v, i, val)                                                    \
    (am_vector_reserve(v, (v)->_size + 1) &&                                           \
     (memmove((v)->_data + i + 1, (v)->_data + i, sizeof((v)->_data[0]) * (v)->_size), \
      (v)->_size++,                                                                    \
      am_vector_at(v, i) = (val), true))
#define am_vector_push_back(v, val)          \
    (am_vector_reserve(v, (v)->_size + 1) && \
     (am_vector_at(v, (v)->_size++) = (val), \
      true))
#define am_vector_emplace(v, i)                                                        \
    (am_vector_reserve(v, (v)->_size + 1) &&                                           \
     (memmove((v)->_data + i + 1, (v)->_data + i, sizeof((v)->_data[0]) * (v)->_size), \
      (v)->_size++,                                                                    \
      am_vector_at(v, i),                                                              \
      true))
#define am_vector_emplace_back(v)         \
    (am_vector_reserve(v, (v)->_size + 1) \
     ? am_vector_at(v, (v)->_size++)      \
     : NULL)

#define am_vector_begin(v)  ((v)->_data)
#define am_vector_end(v)    ((v)->_data + (v)->_size)
#define am_vector_rbegin(v) ((v)->_data + (v)->_size - 1)
#define am_vector_rend(v)   ((v)->_data - 1)

#define am_vector_foreach(it, v) \
    for ((it) = am_vector_begin(v); (it) != am_vector_end(v); (it)++)
#define am_vector_foreach_rev(it, v) \
    for ((it) = am_vector_rbegin(v); (it) != am_vector_rend(v); (it)--)

/*****************************************************************************
 *
 * Implementation
 *
 *****************************************************************************/

#define am__vector_cast(v) \
    &(v)->_size, &(v)->_capacity, (void **)&(v)->_data, sizeof((v)->_data[0])

/* Invariant: n is zero or a power-of-2 */
static AM_INLINE uint32_t am__vector_nextcap(uint32_t n)
{
    return n ? n << 1 : 2;
}

/* Returns the nearest valid capacity that is greater than n */
static AM_INLINE uint32_t am__vector_closest_cap(uint32_t n)
{
    if (n == 0) {
        return 0;
    } else if (n <= 2) {
        return 2;
    } else {
        return 1 << (32 - __builtin_clz(n - 1));
    }
}

AM_ATTR_NON_NULL((1, 2, 3))
static AM_INLINE bool am__vector_reserve(
        uint32_t *size, uint32_t *capacity, void **data, uint32_t sz, uint32_t newsz) {
    (void)size;
    if (*capacity < newsz) {
        void *p;
        uint32_t newcap;
        newcap = am__vector_nextcap(*capacity);
        p = realloc(*data, newcap * sz);
        if (!p) {
            return false;
        }
        *capacity = newcap;
        *data = p;
    }
}

AM_ATTR_NON_NULL((1, 2, 3))
static AM_INLINE bool am__vector_shrink_to_fit(
        uint32_t *size, uint32_t *capacity, void **data, uint32_t sz) {
    if (*size < *capacity) {
        uint32_t newcap;
        newcap = am__vector_closest_cap(*size);
        if (*capacity > newcap) {
            void *p;
            p = realloc(*data, newcap * sz);
            if (!p) {
                return false;
            }
            *capacity = newcap;
            *data = p;
        }
    }
    return true;
}

#endif /* ifndef AM_DATA_VECTOR_H */
