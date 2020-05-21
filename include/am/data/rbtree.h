
#ifndef AM_DATA_RBTREE_H
#define AM_DATA_RBTREE_H 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "am/macros.h"

struct am_rb_root {
    struct am_rb_node *node;
};

#define AM_RB_ROOT_INITIALIZER { NULL }
static AM_INLINE void am_rb_root_init(struct am_rb_root *root);

struct am_rb_node {
    uintptr_t _parent_color; /* private */
    struct am_rb_node *left;
    struct am_rb_node *right;
};
AM_STATIC_ASSERT(AM_ALIGNOF_TYPE(struct am_rb_node) > 2, "am_rb_node alignment too small");

static AM_INLINE void am_rb_node_clear(struct am_rb_node *node);
static AM_INLINE bool am_rb_node_empty(const struct am_rb_node *node);

extern void am_rb_link_node(
        struct am_rb_node *node,
        struct am_rb_node *parent,
        struct am_rb_node **link) AM_ATTR_NON_NULL((1, 3));
extern void am_rb_rebalance(
        struct am_rb_node *node,
        struct am_rb_root *root) AM_ATTR_NON_NULL((1, 2));
extern void am_rb_erase(
        struct am_rb_node *node,
        struct am_rb_root *root) AM_ATTR_NON_NULL((1, 2));
extern void am_rb_replace(
        struct am_rb_node *old,
        struct am_rb_node *new,
        struct am_rb_root *root) AM_ATTR_NON_NULL((1, 2, 3));

extern struct am_rb_node *am_rb_first(const struct am_rb_root *root) AM_ATTR_NON_NULL((1));
extern struct am_rb_node *am_rb_last(const struct am_rb_root *root) AM_ATTR_NON_NULL((1));
extern struct am_rb_node *am_rb_next(const struct am_rb_node *node) AM_ATTR_NON_NULL((1));
extern struct am_rb_node *am_rb_prev(const struct am_rb_node *node) AM_ATTR_NON_NULL((1));

/*****************************************************************************/

static AM_INLINE void am_rb_root_init(struct am_rb_root *root) {
    root->node = NULL;
}
static AM_INLINE void am_rb_node_clear(struct am_rb_node *node) {
    node->_parent_color = (uintptr_t)node;
}
static AM_INLINE bool am_rb_node_empty(const struct am_rb_node *node) {
    return node->_parent_color == (uintptr_t)node;
}

#endif /* ifndef AM_DATA_RBTREE_H */
