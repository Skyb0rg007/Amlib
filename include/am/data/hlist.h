
#ifndef AM_DATA_HLIST_H
#define AM_DATA_HLIST_H 1

#include <stddef.h>
#include <stdbool.h>
#include "am/macros.h"

/** @brief The head of the hlist */
struct am_hlist_head {
    struct am_hlist_node *first;
};

/** @brief Intrusive member of the list */
struct am_hlist_node {
    struct am_hlist_node *next,
                         **pprev;
};

/** @brief Initializer for struct am_hlist_head */
#define AM_HLIST_HEAD_INITIALIZER { NULL }
/** @brief Initializer for struct am_hlist_node */
#define AM_HLIST_NODE_INITIALIZER { NULL, NULL }

/****************************************************************************/

/** @brief Initialize a hlist_head */
AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hlist_head_init(struct am_hlist_head *h)
{
    h->first = NULL;
}

/** @brief Initialize a hlist_node */
AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hlist_node_init(struct am_hlist_node *n)
{
    n->next = NULL;
    n->pprev = NULL;
}

/** @brief Determine if a hlist_head is empty */
AM_ATTR_NON_NULL((1))
static AM_INLINE
bool am_hlist_is_empty(const struct am_hlist_head *h)
{
    return !h->first;
}

/** @brief Delete a node from a list */
AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hlist_del(struct am_hlist_node *n)
{
    struct am_hlist_node *next = n->next;
    struct am_hlist_node **pprev = n->pprev;

    *pprev = next;
    if (next) {
        next->pprev = pprev;
    }
    am_hlist_node_init(n);
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_hlist_add_head(struct am_hlist_node *n, struct am_hlist_head *h)
{
    struct am_hlist_node *first = h->first;

    n->next = first;
    if (first) {
        first->pprev = &n->next;
    }
    h->first = n;
    n->pprev = &h->first;
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_hlist_add_before(struct am_hlist_node *n, struct am_hlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_hlist_add_behind(struct am_hlist_node *n, struct am_hlist_node *prev)
{
    n->next = prev->next;
    prev->next = n;
    n->pprev = &prev->next;

    if (n->next) {
        n->next->pprev = &n->next;
    }
}

/** @brief Iterate over a hlist
 * @param it 'struct am_hlist_node *' iterator
 * @param head 'struct am_hlist_head *' list
 */
#define am_hlist_foreach(it, head) \
    for (it = (head)->first; it != NULL; it = it->next)

/** @brief Iterate over a hlist, safe to deletion
 * @param it 'struct am_hlist_node *' iterator
 * @param tmp 'struct am_hlist_node *' temp
 * @param head 'struct am_hlist_head *' list
 */
#define am_hlist_foreach_safe(it, tmp, head) \
    for (it = (head)->first; it && (tmp = it->next, 1); it = tmp)

#endif /* ifndef AM_DATA_HLIST_H */
