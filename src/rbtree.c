
#include <assert.h>
#include <stdbool.h>
#include "am/data/rbtree.h"

#define RED   0
#define BLACK 1

#define get_parent(node) ((struct am_rb_node *)((node)->_parent_color & ~1))
#define get_color(node)  ((node)->_parent_color & 1)
#define set_red(node)    ((node)->_parent_color &= ~1)
#define set_black(node)  ((node)->_parent_color |= 1)
#define set_parent_color(node, p, c) ((node)->_parent_color = (uintptr_t)(p) | (c))
#define set_parent(node, p) set_parent_color(node, p, get_color(node))

static void change_child(
        struct am_rb_node *old,
        struct am_rb_node *new,
        struct am_rb_node *parent,
        struct am_rb_root *root)
{
    if (parent) {
        if (parent->left == old) {
            parent->left = new;
        } else {
            parent->right = new;
        }
    } else {
        root->node = new;
    }
}

void am_rb_link_node(
        struct am_rb_node *node,
        struct am_rb_node *parent,
        struct am_rb_node **link)
{
    assert(node && link);
    set_parent_color(node, parent, RED);
    node->left = node->right = NULL;
    *link = node;
}

void am_rb_rebalance(
        struct am_rb_node *node,
        struct am_rb_root *root)
{
    assert(node && root && root->node);
    for (;;) {
        struct am_rb_node *parent;  /* Parent of 'node' */
        struct am_rb_node *gparent; /* Grandparent of 'node' */

        parent = get_parent(node);
        if (parent == NULL || get_color(parent) == BLACK) {
            break;
        }

        gparent = get_parent(parent);
        if (!gparent) {
            break;
        }

        if (gparent->left == parent) {
            /* Left-side rebalance */
            struct am_rb_node *uncle; /* Uncle of 'node' */
            uncle = gparent->right;
            if (uncle && get_color(uncle) == RED) {
                /* Case 1 - uncle is red */
                set_black(parent);
                set_black(uncle);
                set_red(gparent);
                node = gparent;
                continue;
            } else {
                if (gparent->right == node) {
                    /* Case 3 - 'node' is a right-child */
                    /* Rotate to turn into case 2 */
                    parent->right = node->left;
                    node->left = parent;
                    gparent->left = node;
                    set_parent(parent, node);
                    if (parent->right) {
                        set_parent(parent->right, parent);
                    }
                    parent = node;
                    /* fallthrough */
                }

                {
                    struct am_rb_node *ggparent; /* Great-grandparent of 'node' */
                    ggparent = get_parent(gparent);

                    /* Case 2 - 'node' is a left-child */
                    set_red(gparent);
                    set_black(parent);
                    gparent->left = parent->right;
                    parent->right = gparent;
                    if (!ggparent) {
                        root->node = parent;
                    } else if (ggparent->left == gparent) {
                        ggparent->left = parent;
                    } else {
                        ggparent->right = parent;
                    }
                    set_parent(parent, get_parent(gparent));
                    set_parent(gparent, parent);
                    if (gparent->left) {
                        set_parent(gparent->left, gparent);
                    }
                    break;
                }
            }
        } else /* if (gparent->right == parent) */ {
            /* Right-side rebalance - symmetric to left-side rebalance */
            struct am_rb_node *uncle; /* Uncle of 'node' */
            uncle = gparent->left;
            if (uncle && get_color(uncle) == RED) {
                /* Case 1 - uncle is red */
                set_black(parent);
                set_black(uncle);
                set_red(gparent);
                node = gparent;
                continue;
            } else {
                if (gparent->left == node) {
                    /* Case 3 - 'node' is a left-child */
                    /* Rotate to turn into case 2 */
                    parent->left = node->right;
                    node->right = parent;
                    gparent->right = node;
                    set_parent(parent, node);
                    if (parent->left) {
                        set_parent(parent->left, parent);
                    }
                    parent = node;
                    /* fallthrough */
                }

                {
                    /* Case 2 - 'node' is a right-child */
                    struct am_rb_node *ggparent; /* Great-grandparent of 'node' */
                    ggparent = get_parent(gparent);

                    set_red(gparent);
                    set_black(parent);
                    gparent->right = parent->left;
                    parent->left = gparent;
                    if (!ggparent) {
                        root->node = parent;
                    } else if (ggparent->left == gparent) {
                        ggparent->left = parent;
                    } else {
                        ggparent->right = parent;
                    }
                    set_parent(parent, get_parent(gparent));
                    set_parent(gparent, parent);
                    if (gparent->right) {
                        set_parent(gparent->right, gparent);
                    }
                    break;
                }
            }
        }
    }
    set_black(root->node);
}

static void rb_change_child(
        struct am_rb_node *old,
        struct am_rb_node *new,
        struct am_rb_node *parent,
        struct am_rb_root *root)
{
    if (parent) {
        if (parent->left == old) {
            parent->left = new;
        } else {
            parent->right = new;
        }
    } else {
        root->node = new;
    }
}

static struct am_rb_node *rb_erase_helper(
        struct am_rb_node *node,
        struct am_rb_root *root)
{
    if (!node->left && !node->right) {
        /* Case 1-a: Node has 0 children */
        struct am_rb_node *parent = get_parent(node);
        rb_change_child(node, node->right, parent, root);
        return get_color(node) == BLACK ? parent : NULL;
    } else if (!node->left) {
        /* Case 1-b: Node has 1 right child */
        struct am_rb_node *parent = get_parent(node);
        rb_change_child(node, node->right, parent, root);
        node->right->_parent_color = node->_parent_color;
        return NULL;
    } else if (!node->right) {
        /* Case 1-c: Node has 1 left child */
        struct am_rb_node *parent = get_parent(node);
        rb_change_child(node, node->left, parent, root);
        node->left->_parent_color = node->_parent_color;
        return NULL;
    } else if (!node->right->left) {
        /* Case 2: Node's successor is the right child */
        /*    (n)          (s)
              / \          / \
            (x) (s)  ->  (x) (c)
                  \
                  (c)
         */
        struct am_rb_node *x = node->left;
        struct am_rb_node *s = node->right;
        struct am_rb_node *c = s->right;

        s->left = x;
        set_parent(x, s);
        rb_change_child(node, s, get_parent(node), root);
        if (c) {
            set_black(c);
            return NULL;
        } else {
            return get_color(c) == BLACK ? s : NULL;
        }
    } else {
        /* Case 3: Node's successor is leftmost under the node's right subtree */
        /*    (n)          (s)
              / \          / \
            (x) (y)  ->  (x) (y)
                /            /
              (p)          (p)
              /            /
            (s)          (c)
              \
              (c)
         */
        /* TODO */
        /* struct am_rb_node *x = node->left; */
        /* struct am_rb_node *y = node->right; */
        /* struct am_rb_node *c; */

        /* struct am_rb_node *p = y; */
        /* struct am_rb_node *s = p->left; */
        /* while (s->left) { */
            /* p = s; */
            /* s = s->left; */
        /* } */
    }
}

static void rb_erase_color(
        struct am_rb_node *node,
        struct am_rb_root *root)
{

}

void am_rb_erase(
        struct am_rb_node *node,
        struct am_rb_root *root)
{
    struct am_rb_node *rebalance = rb_erase_helper(node, root);
    if (rebalance) {
        rb_erase_color(rebalance, root);
    }
}

void am_rb_replace(
        struct am_rb_node *old,
        struct am_rb_node *new,
        struct am_rb_root *root)
{
    struct am_rb_node *parent;
    assert(old && new && root);

    parent = get_parent(old);
    *new = *old;
    if (old->left) {
        set_parent_color(old->left, new, get_color(old->left));
    }
    if (old->right) {
        set_parent_color(old->right, new, get_color(old->right));
    }
    change_child(old, new, parent, root);
    am_rb_node_clear(old);
}

struct am_rb_node *am_rb_first(const struct am_rb_root *root)
{
    struct am_rb_node *n; 
    assert(root);
    n = root->node;
    if (!n)
        return NULL;
    while (n->left)
        n = n->left;
    return n;
}
struct am_rb_node *am_rb_last(const struct am_rb_root *root)
{
    struct am_rb_node *n; 
    assert(root);
    n = root->node;
    if (!n)
        return NULL;
    while (n->right)
        n = n->right;
    return n;
}
struct am_rb_node *am_rb_next(const struct am_rb_node *node)
{
    struct am_rb_node *parent;
    assert(node);

    if (am_rb_node_empty(node))
        return NULL;

    if (node->right) {
        node = node->right;
        while (node->left)
            node = node->left;
        return (struct am_rb_node *)node;
    }

    while ((parent = get_parent(node)) && node == parent->right)
        node = parent;
    return parent;
}
struct am_rb_node *am_rb_prev(const struct am_rb_node *node)
{
    struct am_rb_node *parent;
    assert(node);

    if (am_rb_node_empty(node))
        return NULL;

    if (node->left) {
        node = node->left;
        while (node->right)
            node = node->right;
        return (struct am_rb_node *)node;
    }

    while ((parent = get_parent(node)) && node == parent->left)
        node = parent;
    return parent;
}

