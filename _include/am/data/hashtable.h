
#ifndef AM_DATA_HASHTABLE_H
#define AM_DATA_HASHTABLE_H 1

#include <stdio.h>
#include "am/data/hlist.h"
#include "am/macros.h"

#define am_hashtable(bits) \
    struct { struct am_hlist_head _raw[1 << (bits)]; }

/** @brief Initialize a hashtable */
#define am_hashtable_init(ht) am__hashtable_init((ht)->_raw, AM_ARRAY_SIZE((ht)->_raw))

static AM_INLINE
void am__hashtable_init(struct am_hlist_head *ht, size_t sz)
{
    size_t i;
    for (i = 0; i < sz; i++)
        am_hlist_head_init(&ht[i]);
}

#define am_hashtable_add(ht, key, node) \
    am_hlist_add_head(node, &(ht)->_raw[(key) % AM_ARRAY_SIZE((ht)->_raw)])

/** @brief Determine if the hashtable is empty
 * @return true if the hashtable is empty
 */
#define am_hashtable_is_empty(ht) \
    am__hashtable_is_empty((ht)->_raw, AM_ARRAY_SIZE((ht)->_raw))

static AM_INLINE
bool am__hashtable_is_empty(const struct am_hlist_head *ht, size_t sz)
{
    size_t i;
    for (i = 0; i < sz; i++)
        if (!am_hlist_is_empty(&ht[i]))
            return false;

    return true;
}

/** @brief Remove an object from a hashtable */
static AM_INLINE
void am_hashtable_del(struct am_hlist_node *node)
{
    am_hlist_del(node);
}

/** @brief Iterate over a hashtable
 * @param ht The hashtable to iterate over
 * @param bkt Bucket to use as a loop cursor
 * @param it 'struct am_hlist_node *' to use as iterator
 */
#define am_hashtable_foreach(ht, bkt, it) \
    for ((bkt) = 0, (it) = NULL; \
            (it) == NULL && (bkt) < AM_ARRAY_SIZE((ht)->_raw); \
            (bkt)++) \
        am_hlist_foreach(it, &(ht)->_raw[bkt])

/** @brief Iterate over a hashtable, safe to deletion
 * @param ht The hashtable to iterate over
 * @param bkt Bucket to use as a loop cursor
 * @param tmp 'struct am_hlist_node *' to use as temporary
 * @param it 'struct am_hlist_node *' to use as iterator
 */
#define am_hashtable_foreach_safe(ht, bkt, tmp, it) \
    for ((bkt) = 0, (it) = NULL; \
            (it) == NULL && (bkt) < AM_ARRAY_SIZE((ht)->_raw); \
            (bkt)++) \
        am_hlist_foreach_safe(it, tmp, &(ht)->_raw[bkt])

#endif /* ifndef AM_DATA_HASHTABLE_H */
