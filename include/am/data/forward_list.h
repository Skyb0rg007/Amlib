/** @file am/data/forward_list.h
 * @brief Intrusive singly-linked list
 */

#ifndef AM_DATA_FORWARD_LIST_H
#define AM_DATA_FORWARD_LIST_H 1

#include "am/common.h"

struct am_forward_list;
struct am_forward_list_node {
    struct am_forward_list_node *next;
};

/* #define AM_FORWARD_LIST_INITIALIZER */
/* #define AM_FORWARD_LIST_NODE_INITIALIZER */
static AM_INLINE void am_forward_list_init(struct am_forward_list *l) AM_ATTR_NONNULL((1));
static AM_INLINE void am_forward_list_node_init(struct am_forward_list_node *n) AM_ATTR_NONNULL((1));

/* Iteration */
/* struct am_forward_list_node *am_forward_list_begin(struct am_forward_list *l) */
/* struct am_forward_list_node *am_forward_list_end(struct am_forward_list *l) */
/* struct am_forward_list_node *am_forward_list_node_next(struct am_forward_list_node *n) */
/* am_forward_list_foreach(
        struct am_forward_list_node *it,
        struct am_forward_list *l) */
/* am_forward_list_foreach_safe(
        struct am_forward_list_node *it,
        struct am_forward_list_node *tmp,
        struct am_forward_list *l) */

/* Query */
static AM_INLINE bool am_forward_list_empty(const struct am_forward_list *l) AM_ATTR_NONNULL((1));
static AM_INLINE size_t am_forward_list_size(const struct am_forward_list *l) AM_ATTR_NONNULL((1));

/* Modification */
/* Insert before a given node */
static AM_INLINE void am_forward_list_insert_after(struct am_forward_list_node *pos, struct am_forward_list_node *value) AM_ATTR_NONNULL((1, 2));
/* Insert at the front */
static AM_INLINE void am_forward_list_push_front(struct am_forward_list *l, struct am_forward_list_node *value) AM_ATTR_NONNULL((1, 2));
/* Erase the list node after the given one from the list */
static AM_INLINE void am_forward_list_node_erase_after(struct am_forward_list_node *n) AM_ATTR_NONNULL((1));

/*****************************************************************************
 *
 * Implementation
 *
 *****************************************************************************/

struct am_forward_list {
    struct am_forward_list_node *head;
};

#define AM_FORWARD_LIST_INITIALIZER      { NULL }
#define AM_FORWARD_LIST_NODE_INITIALIZER { NULL }

static AM_INLINE void am_forward_list_init(struct am_forward_list *l)
{
    l->head = NULL;
}

static AM_INLINE void am_forward_list_node_init(struct am_forward_list_node *n)
{
    n->next = NULL;
}

#define am_forward_list_begin(l)     ((l)->head)
#define am_forward_list_end(l)       NULL
#define am_forward_list_node_next(l) ((l)->next)

#define am_forward_list_foreach(it, l)                       \
    for ((it) = (l)->head; (it) != NULL; (it) = (it)->next)
#define am_forward_list_foreach_safe(it, tmp, l)             \
    for ((it) = (l)->head, (tmp) = (it) ? (it)->next : NULL; \
         (it) != NULL;                                       \
         (it) = (tmp), (tmp) = (tmp) ? (tmp)->next : NULL)

static AM_INLINE bool am_forward_list_empty(const struct am_forward_list *l)
{
    return l->head == NULL;
}
static AM_INLINE size_t am_forward_list_size(const struct am_forward_list *l)
{
    size_t sz = 0;
    struct am_forward_list_node *it;
    am_forward_list_foreach(it, l)
        sz++;
    return sz;
}

static AM_INLINE void am_forward_list_insert_after(
        struct am_forward_list_node *pos,
        struct am_forward_list_node *value)
{
    struct am_forward_list_node *next;
    next = pos->next;
    value->next = next;
    pos->next = value;
}

static AM_INLINE void am_forward_list_push_front(
        struct am_forward_list *l,
        struct am_forward_list_node *value)
{
    value->next = l->head;
    l->head = value;
}

static AM_INLINE void am_forward_list_node_erase_after(struct am_forward_list_node *n)
{
    n->next = n->next->next;
}

#endif /* ifndef AM_DATA_FORWARD_LIST_H */
