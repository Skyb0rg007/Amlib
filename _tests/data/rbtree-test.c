
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "am/macros.h"
#include "am/utils.h"
#include "am/data/rbtree.h"

struct Person {
    int id;
    char name[20];
    char email[20];
    struct am_rb_node node;
};

static
struct Person *alloc_person(int id, const char *name, const char *email)
{
    struct Person *p = malloc(sizeof(struct Person));
    assert(p);
    p->id = id;
    strncpy(p->name, name, 19); p->name[19] = '\0';
    strncpy(p->email, email, 19); p->email[19] = '\0';
    am_rb_node_clear(&p->node);
    return p;
}

static
struct Person *person_search(struct am_rb_root *root, char *name)
{
    struct am_rb_node *node = root->node;

    while (node) {
        struct Person *data = AM_CONTAINER_OF(node, struct Person, node);
        int result = strcmp(name, data->name);

        if (result < 0)
            node = node->left;
        else if (result > 0)
            node = node->right;
        else
            return data;
    }
    return NULL;
}

static
bool person_insert(struct am_rb_root *root, struct Person *data)
{
    struct am_rb_node **link = &(root->node);
    struct am_rb_node *parent = NULL;

    while (*link) {
        struct Person *this = AM_CONTAINER_OF(*link, struct Person, node);
        int result = strcmp(data->name, this->name);

        parent = *link;
        if (result < 0) {
            link = &((*link)->left);
        } else if (result > 0) {
            link = &((*link)->right);
        } else {
            return false;
        }
    }

    am_rb_link_node(&data->node, parent, link);
    am_rb_rebalance(&data->node, root);
    return true;
}

static
bool person_erase(struct am_rb_root *root, char *name)
{
    struct Person *data = person_search(root, name);
    if (data) {
        am_rb_erase(&data->node, root);
        free(data);
        return true;
    }
    return false;
}

int main(void)
{
    int i;
    struct am_rb_root tree = AM_RB_ROOT_INITIALIZER;

    for (i = 0; i < 300; i++) {
        struct Person *p = alloc_person(i, "", "");
        sprintf(p->name, "Person %d", i);
        sprintf(p->email, "person%d@gmail.com", i);
        person_insert(&tree, p);
    }

    {
        struct am_rb_node *node = am_rb_first(&tree);
        while (node) {
            struct Person *p;
            p = AM_CONTAINER_OF(node, struct Person, node);
            printf("Person { .id = %d, .name = %s, .email = %s }\n", p->id, p->name, p->email);
            node = am_rb_next(node);
            am_rb_erase(&p->node, &tree);
            free(p);
        }
    }

    return 0;
}

