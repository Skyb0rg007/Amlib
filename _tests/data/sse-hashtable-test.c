
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "am/macros.h"
#include "am/data/sse_hashtable.h"

struct Person {
    int id;
    uint32_t name_hash;
    char name[30];
    char email[30];
};

struct PersonHashtable {
    size_t size, capacity;
    struct group_meta *meta;
    struct Person *elems;
};

static
void person_ht_init(struct PersonHashtable *ht)
{
    void *data;
    size_t sz = 32;

    data = malloc(sizeof(struct group_meta) * sz + sizeof(struct Person) * sz);
    assert(data != NULL);

    ht->size = 0;
    ht->capacity = 32;
    ht->meta = data;
    ht->elems = (void *)((char *)data + sizeof(struct group_meta) * sz);
}

static
void person_ht_free(struct PersonHashtable *ht)
{
    free(ht->meta);
    memset(ht, 0x0, sizeof *ht);
}

static
struct Person *person_ht_find(
        struct PersonHashtable *ht,
        const char *name,
        uint32_t name_hash)
{
    size_t group = H1(name_hash) % ht->size;
    while (true)
    {
        struct group_meta *g = &ht->meta[group];
        uint32_t it;

        bitmask_foreach(it, group_matches(g, H2(name_hash))) {
            size_t idx = bitmask_iterator_to_idx(it);
            if (strcmp(name, ht->elems[idx].name) == 0) {
                return &ht->elems[idx];
            }
        }

        if (group_is_full(g)) {
            return NULL;
        }

        group = (group + 1) % ht->size;
    }
}

bool person_ht_insert(
        struct PersonHashtable *ht,
        const struct Person *p)
{
    uint32_t hash = p->name_hash;
    size_t group = hash % ht->size;
    while (true)
    {
        struct group_meta *g = &ht->meta[group];
        uint32_t it;

        bitmask_foreach(it, group_matches(g, H2(hash))) {
            size_t idx = bitmask_iterator_to_idx(it);
        }
    }
}

