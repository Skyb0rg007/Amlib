
#ifndef AM_LOCKFREE_ARRAY_H
#define AM_LOCKFREE_ARRAY_H 1

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "am/alloc.h"
#include "am/atomic.h"

struct am__spmc_array {
    am_atomic_uint num_committed;
    unsigned length;
    void *values[1]; /* Flexible array member emulation */
};

struct am_spmc_array {
    struct am_alloc allocator;
    struct am__spmc_array *active;
    unsigned num_entries;
    struct am__spmc_array *transaction;
};

struct am_spmc_array_iterator {
    struct am__spmc_array *snapshot;
};

/** @brief Initialize an array */
AM_ATTR_NON_NULL((1, 2)) AM_PUBLIC
bool am_spmc_array_init(struct am_spmc_array *arr, struct am_alloc *allocator, unsigned length);

/** @brief Commit changes made to the array */
AM_ATTR_NON_NULL((1)) AM_PUBLIC
bool am_spmc_array_commit(struct am_spmc_array *arr);

/** @brief Add an element to the array */
AM_ATTR_NON_NULL((1)) AM_PUBLIC
bool am_spmc_array_put(struct am_spmc_array *arr, void *val);

/** @brief Add an element to the array, if it doesn't already exist */
AM_ATTR_NON_NULL((1)) AM_PUBLIC
bool am_spmc_array_put_unique(struct am_spmc_array *arr, void *val);

/** @brief Remove an element from the array */
AM_ATTR_NON_NULL((1)) AM_PUBLIC
bool am_spmc_array_remove(struct am_spmc_array *arr, void *val);

/** @brief Deallocate an array */
AM_ATTR_NON_NULL((1)) AM_PUBLIC
bool am_spmc_array_destroy(struct am_spmc_array *arr);

/** @brief Get the length of the array */
AM_ATTR_NON_NULL((1)) AM_PUBLIC
unsigned am_spmc_array_length(struct am_spmc_array *arr)
{
    struct am__spmc_array *a = am_atomic_load_ptr((void **)&arr->active);
    return am_atomic_load_uint(&a->num_committed);
}

/** @brief Get access to the underlying buffer */

#endif /* ifndef AM_LOCKFREE_ARRAY_H */
