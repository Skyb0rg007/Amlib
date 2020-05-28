
#include "am/data/rbtree.h"
#include "am/macros.h"

#define rb_parent(node)     ((struct am_rb_node *)((node)->_parent_color & ~1))
#define rb_red_parent(node) ((struct am_rb_node *)(node)->_parent_color)
#define rb_color(node)      ((node)->_parent_color & 1)
#define rb_set_pc(node, parent, color) \
    (node)->_parent_color = (intptr_t)(parent) | color

struct am_rb_node *am_rb_next(const struct am_rb_node *node)
{
    struct am_rb_node *parent;

    if (am_rb_node_empty(node)) {
        return NULL;
    }

    if (node->right != NULL) {
        node = node->right;
        while (node->left)
            node = node->left;
        return (struct am_rb_node *)node;
    }

    while ((parent = rb_parent(node)) && node == parent->right)
        node = parent;

    return parent;
}

struct am_rb_node *am_rb_prev(const struct am_rb_node *node)
{
    struct am_rb_node *parent;

    if (am_rb_node_empty(node)) {
        return NULL;
    }

    if (node->left) {
        node = node->left;
        while (node->right)
            node = node->right;
        return (struct am_rb_node *)node;
    }

    while ((parent = rb_parent(node)) && node == parent->left)
        node = parent;

    return parent;
}

struct am_rb_node *am_rb_first(const struct am_rb_root *root)
{
    struct am_rb_node *node;

    node = root->node;
    if (!node)
        return NULL;
    while (node->left)
        node = node->left;
    return node;
}

struct am_rb_node *am_rb_last(const struct am_rb_root *root)
{
    struct am_rb_node *node;

    node = root->node;
    if (!node)
        return NULL;
    while (node->right)
        node = node->right;
    return node;
}

static void rb_insert(
        struct am_rb_node *node,
        struct am_rb_root *root)
{
    struct am_rb_node *parent = rb_red_parent(node);
    struct am_rb_node *gparent = NULL;
    struct am_rb_node *tmp = NULL;

    while (true) {

        if (AM_UNLIKELY(!parent)) {
            node->_parent_color = 0 | AM_RB_BLACK;
            break;
        }

        if (rb_color(parent) == AM_RB_BLACK) {
            break;
        }

        gparent = rb_red_parent(parent);
        tmp = gparent->right;
        if (parent != tmp) {
            if (tmp && rb_color(tmp) == AM_RB_RED) {
                rb_set_pc(tmp, gparent, AM_RB_BLACK);
                rb_set_pc(parent, gparent, AM_RB_BLACK);
                node = gparent;
                parent = rb_parent(node);
                rb_set_pc(node, parent, AM_RB_RED);
                continue;
            }

            tmp = parent->right;
            if (node == tmp) {
                tmp = node->left;
                parent->right = tmp;
                node->left = parent;
                if (tmp) {
                    rb_set_pc(tmp, parent, AM_RB_BLACK);
                }
                rb_set_pc(parent, node, AM_RB_RED);
                parent = node;
                tmp = node->right;
            }

            gparent->left = tmp;
            parent->right = gparent;
            if (tmp) {
                rb_set_pc(tmp, gparent, AM_RB_BLACK);
            }
            rb_rotate_set_parents(gparent, parent, root, AM_RB_RED);
            break;
        } else {

        }
    }
}

void am_rb_insert(
        struct am_rb_node *new_node,
        struct am_rb_node *parent,
        struct am_rb_node **link,
        struct am_rb_root *root)
{
    new_node->_parent_color = (uintptr_t)parent;
    new_node->left = new_node->right = NULL;
    *link = new_node;

    rb_insert(new_node, root);
}

void am_rb_erase(struct am_rb_node *node, struct am_rb_root *root);
