
/*
 * Intrusive doubly-linked list data structure
 */

#ifndef AM_DATA_LIST_H
#define AM_DATA_LIST_H 1

#include <stddef.h>
#include <stdbool.h>
#include "am/macros.h"

struct am_list;
/* To be embedded. Also acts as iterator. */
struct am_list_node {
    struct am_list_node *prev;
    struct am_list_node *next;
};


/* Initialization */
static AM_INLINE void am_list_init(struct am_list *l);
static AM_INLINE void am_list_node_init(struct am_list_node *n);

/* Iteration */
/* struct am_list_node *am_list_begin(struct am_list *l) */
/* struct am_list_node *am_list_end(struct am_list *l) */
/* struct am_list_node *am_list_node_next(struct am_list_node *n) */
/* struct am_list_node *am_list_node_prev(struct am_list_node *n) */
/* am_list_foreach(struct am_list_node *it, struct am_list *l) */
/* am_list_foreach_safe(struct am_list_node *it, struct am_list_node *tmp, struct am_list *l) */
/* am_list_foreach_rev(struct am_list_node *it, struct am_list *l) */
/* am_list_foreach_rev_safe(struct am_list_node *it, struct am_list_node *tmp, struct am_list *l) */

/* Query */
static AM_INLINE bool am_list_empty(const struct am_list *l);
static AM_INLINE size_t am_list_size(const struct am_list *l);

/* Modification */
/* Insert before a given node */
static AM_INLINE void am_list_insert(struct am_list_node *pos, struct am_list_node *value);
/* Insert at the back */
static AM_INLINE void am_list_push_back(struct am_list *l, struct am_list_node *value);
/* Insert at the front */
static AM_INLINE void am_list_push_front(struct am_list *l, struct am_list_node *value);
/* Erase the list node from the list */
static AM_INLINE void am_list_node_erase(struct am_list_node *n);

/*****************************************************************************
 *
 * Implementation
 *
 *****************************************************************************/

struct am_list {
    struct am_list_node _head;
};

static AM_INLINE void am_list_node_init(struct am_list_node *n) {
    n->prev = n->next = n;
}
static AM_INLINE void am_list_init(struct am_list *l) {
    am_list_node_init(&l->_head);
}

#define am_list_begin(l)  ((l)->_head.next)
#define am_list_end(l)    (&(l)->_head)
#define am_list_rbegin(l) ((l)->_head.prev)
#define am_list_rend(l)   (&(l)->_head)

#define am_list_node_next(n) ((n)->next)
#define am_list_node_prev(n) ((n)->prev)
#define am_list_foreach(it, l)                                               \
    for ((it) = am_list_begin(l);                                            \
         (it) != am_list_end(l);                                             \
         (it) = am_list_node_next(it))
#define am_list_foreach_safe(it, tmp, l)                                     \
    for ((it) = am_list_begin(l), (tmp) = am_list_node_next(it);             \
         (it) != am_list_end(l);                                             \
         (it) = (tmp), (tmp) = am_list_node_next(tmp))
#define am_list_foreach_rev(it, l)                                           \
    for ((it) = am_list_rbegin(l);                                           \
         (it) != am_list_rend(l);                                            \
         (it) = am_list_node_prev(it))
#define am_list_foreach_rev_safe(it, tmp, l)                                 \
    for ((it) = am_list_rbegin(l), (tmp) = am_list_node_prev(it);            \
         (it) != am_list_rend(l);                                            \
         (it) = (tmp), (tmp) = am_list_node_prev(tmp))

static AM_INLINE bool am_list_empty(const struct am_list *l)
{
    return l->_head.next == l->_head.prev;
}

static AM_INLINE size_t am_list_size(const struct am_list *l)
{
    size_t sz = 0;
    const struct am_list_node *it;
    am_list_foreach(it, l)
        sz++;
    return sz;
}

static AM_INLINE void am_list_insert(
        struct am_list_node *next,
        struct am_list_node *value)
{
    struct am_list_node *prev = am_list_node_prev(next);
    value->next = next;
    value->prev = prev;
    prev->next = value;
    next->prev = value;
}

static AM_INLINE void am_list_push_back(
        struct am_list *l,
        struct am_list_node *value)
{
    am_list_insert(am_list_end(l), value);
}
static AM_INLINE void am_list_push_front(
        struct am_list *l,
        struct am_list_node *value)
{
    am_list_insert(am_list_begin(l), value);
}
static AM_INLINE void am_list_node_erase(struct am_list_node *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;
    am_list_node_init(n);
}

#endif /* ifndef AM_DATA_LIST_H */
