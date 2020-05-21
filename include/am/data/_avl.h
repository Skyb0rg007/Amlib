
#ifndef AM_DATA_AVL_H
#define AM_DATA_AVL_H 1

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "am/macros.h"

struct am_avl;
struct am_avl_node {
    struct am_avl_node *left;
    struct am_avl_node *right;
    struct am_avl_node *parent;
    uintptr_t balance;
};

/* #define AM_AVL_INITIALIZER */
static AM_INLINE void am_avl_init(struct am_avl *t);
static AM_INLINE void am_avl_node_init(struct am_avl_node *n);

/* Searching - must be written yourself */

/*****************************************************************************
 *
 * Implementation
 *
 *****************************************************************************/

struct am_avl {
    struct am_avl_node *head;
};

#define AM_AVL_INITIALIZER { NULL }

static AM_INLINE void am_avl_init(struct am_avl *t)
{
    t->head = NULL;
}

static AM_INLINE void am_avl_node_init(struct am_avl_node *n)
{
    n->left = n->right = n->parent = NULL;
    n->balance = 0;
}

#endif /* ifndef AM_DATA_AVL_H */

