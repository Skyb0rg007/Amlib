
#include <assert.h>
#include <stdio.h>
#include "am/common.h"
#include "am/logging.h"
#include "am/alloc.h"
#include "am/data/vector.h"

/* POD */

struct person {
    char name[50];
};

void person_test(void)
{
    int i;
    struct person *it;
    am_vector(struct person) v;
    am_vector_init(&v);

    assert(am_vector_size(&v) == 0);
    assert(am_vector_capacity(&v) == 0);

    am_vector_reserve_pod(&v, 12);
    assert(am_vector_size(&v) == 0);
    assert(am_vector_capacity(&v) == 12);

    for (i = 0; i < 20; i++) {
        struct person *p = am_vector_emplace_back_pod(&v);
        assert(p != NULL);
        sprintf(p->name, "Person %2.2d", i);
        p->name[10] = '\0';
    }

    i = 0;
    am_vector_foreach(it, &v) {
        int indent = 0;
        printf("v[%d] = %n", i++, &indent);
        printf("%*s{ .name = \"%.50s\" }\n", 8 - indent, "", it->name);
    }

    am_vector_free_pod(&v);
}

struct selfish {
    void *self;
    int x;
};

static void selfish_relocate(void *selfish, size_t sz, intptr_t old, void *ud)
{
    (void)sz; (void)old; (void)ud;
    ((struct selfish *)selfish)->self = selfish;
}


void self_test(void)
{
    int i;
    struct selfish *it;
    am_vector(struct selfish) v;
    static const struct am_vector_info info = {
        am_alloc_standard,
        { selfish_relocate, NULL }
    };

    am_vector_init(&v);

    for (i = 0; i < 20; i++) {
        struct selfish *s = am_vector_emplace_back(&v, &info);
        assert(s != NULL);
        s->self = s;
        s->x = i;
    }

    i = 0;
    am_vector_foreach(it, &v) {
        printf("(%p) { .self = %p, .x = %d }\n", (void *)it, it->self, it->x);
    }

    am_vector_free(&v, &info);
}

int main(void)
{
    person_test();
    self_test();
    return 0;
}

