
#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "am/data/hash.h"
#include "am/data/hashtable.h"

struct person {
    char name[20];
    int id;
    struct am_hlist_node h;
};

int main(void)
{
    int i;
    am_hashtable(4) ht;

    am_hashtable_init(&ht);
    for (i = 0; i < 200; i++) {
        struct person *p = malloc(sizeof *p);
        uint32_t hash;
        assert(p != NULL);

        snprintf(p->name, sizeof p->name, "Person %d", i);
        p->id = i;
        am_hlist_node_init(&p->h);

        hash = am_hash_fnva1_32(p->name, strlen(p->name));
        am_hashtable_add(&ht, hash, &p->h);
    }

    {
        size_t bkt;
        struct am_hlist_node *it, *tmp;
        am_hashtable_foreach(&ht, bkt, it) {
            struct person *p = AM_CONTAINER_OF(it, struct person, h);
            printf("{ .name = %s, .id = %d }\n", p->name, p->id);
        }

        am_hashtable_foreach_safe(&ht, bkt, tmp, it) {
            struct person *p = AM_CONTAINER_OF(it, struct person, h);
            am_hashtable_del(&p->h);
            free(p);
        }
    }

    return 0;
}

