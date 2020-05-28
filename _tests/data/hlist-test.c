
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "am/macros.h"
#include "am/data/hlist.h"

struct person {
    int id;
    struct am_hlist_node l;
    char name[30];
};

int main(void)
{
    int i;
    struct am_hlist_head head;
    struct am_hlist_node *it, *tmp;

    am_hlist_head_init(&head);
    for (i = 0; i < 20; i++) {
        struct person *p = malloc(sizeof *p);
        assert(p != NULL);
        sprintf(p->name, "Person %i", i);
        p->id = i;
        am_hlist_add_head(&p->l, &head);
    }

    am_hlist_foreach(it, &head) {
        struct person *p = AM_CONTAINER_OF(it, struct person, l);
        printf("{ .name = \"%s\", .id = %d }\n", p->name, p->id);
    }

    am_hlist_foreach_safe(it, tmp, &head) {
        struct person *p = AM_CONTAINER_OF(it, struct person, l);
        am_hlist_del(&p->l);
        free(p);
    }

    return 0;
}
