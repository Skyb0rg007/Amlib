
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "am/macros.h"
#include "am/data/list.h"

struct person {
    int id;
    struct am_list l;
    char name[30];
};

int main(void)
{
    int i;
    struct am_list head;
    struct am_list *it, *tmp;

    am_list_head_init(&head);
    for (i = 0; i < 20; i++) {
        struct person *p = malloc(sizeof *p);
        assert(p != NULL);
        sprintf(p->name, "Person %i", i);
        p->id = i;
        am_list_add(&p->l, &head);
    }

    am_list_foreach(it, &head) {
        struct person *p = AM_CONTAINER_OF(it, struct person, l);
        printf("{ .name = \"%s\", .id = %d }\n", p->name, p->id);
    }

    am_list_foreach_safe(it, tmp, &head) {
        struct person *p = AM_CONTAINER_OF(it, struct person, l);
        am_list_del(&p->l);
        free(p);
    }

    return 0;
}
