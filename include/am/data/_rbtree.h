
#ifndef AM_DATA_RBTREE_H
#define AM_DATA_RBTREE_H 1

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "am/macros.h"

/* Intrusive struct member. Public members are listed. */
struct am_rb_node; /* {
    struct am_rb_node *left;
    struct am_rb_node *right;
}; */

/* Initialize */
static AM_INLINE void am_rb_node_init(struct am_rb_node *node);
static AM_INLINE bool am_rb_node_empty(const struct am_rb_node *node);

/* Root of a Red-Black Tree */
struct am_rb_root {
    struct am_rb_node *node;
};
/* Initializer */
#define AM_RB_ROOT_INITIALIZER
/* Initialize */
static AM_INLINE void am_rb_root_init(struct am_rb_root *root);

/*** Functions for writing insert/delete */
/* Insert a node. */
extern void am_rb_insert(
        struct am_rb_node *new_node, /* The node to insert. Need not be initialized. */
        struct am_rb_node *parent, /* Parent of the new node */
        struct am_rb_node **link, /* Pointer to the child of 'parent' that the node will be inserted into */
        struct am_rb_root *root); /* Root node */
/* Erase a node. */
extern void am_rb_erase(struct am_rb_node *node, struct am_rb_root *root);

/*** Iteration */
extern struct am_rb_node *am_rb_next(const struct am_rb_node *node);
extern struct am_rb_node *am_rb_prev(const struct am_rb_node *node);
extern struct am_rb_node *am_rb_first(const struct am_rb_root *root);
extern struct am_rb_node *am_rb_last(const struct am_rb_root *root);

/*****************************************************************************
 *
 * Usage
 *
 *****************************************************************************/

/*
struct mytype *my_search(struct am_rb_root *root, char *key)
{
    struct am_rb_node *node = root->node;

    while (node) {
        struct mytype *data = AM_CONTAINER_OF(node, struct mytype, node);
        int result = strcmp(key, data->keystring);

        if (result < 0)
            node = node->left;
        else if (result > 0)
            node = node->right;
        else
            return data;
    }
    return NULL;
}

bool my_insert(struct am_rb_root *root, struct mytype *data)
{
    struct am_rb_node **link = &(root->node);
    struct am_rb_node *parent = NULL;

    while (*link) {
        struct mytype *this = AM_CONTAINER_OF(*link, struct mytype, node);
        int result = strcmp(data->keystring, this->keystring);

        parent = *link;
        if (result < 0)
            link = &((*link)->left);
        else if (result > 0)
            link = &((*link)->right);
        else
            return false;
    }

    am_rb_insert(&data->node, parent, link, root);
}

bool my_erase(struct am_rb_root *root, char *key)
{
    struct mytype *data = my_search(root, key);
    if (data) {
        am_rb_erase(&data->node, root);
        mytype_free(data);
        return true;
    }
    return false;
}
*/

/*****************************************************************************
 *
 * Implementation
 *
 *****************************************************************************/

#define AM_RB_RED   0
#define AM_RB_BLACK 1

struct am_rb_node {
    uintptr_t _parent_color; /* Last bit is the current node's color */
    struct am_rb_node *left;
    struct am_rb_node *right;
};
AM_STATIC_ASSERT(AM_ALIGNOF_TYPE(struct am_rb_node) > 2, "");

static AM_INLINE void am_rb_node_init(struct am_rb_node *node)
{
    node->_parent_color = (uintptr_t)node;
    node->left = node->right = NULL;
}
static AM_INLINE bool am_rb_node_empty(const struct am_rb_node *node)
{
    return node->_parent_color == (uintptr_t)node;
}

#undef AM_RB_ROOT_INITIALIZER
#define AM_RB_ROOT_INITIALIZER { NULL }
static AM_INLINE void am_rb_root_init(struct am_rb_root *root)
{
    root->node = NULL;
}

static AM_INLINE void am_rb_node_set_parent(struct am_rb_node *node, struct am_rb_node *parent)
{
    assert((node->_parent_color & 1) == AM_RB_RED); /* Ensure red */
    node->_parent_color = (uintptr_t)parent;
}

#endif /* ifndef AM_DATA_RBTREE_H */
