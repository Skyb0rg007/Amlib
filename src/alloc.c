
#include <stdlib.h>
#include <stdint.h>
#include "am/alloc.h"

static
void *alloc_default(void *ptr, size_t oldsz, size_t newsz, struct am_alloc *self)
{
    (void)self;
    (void)oldsz;

    if (newsz == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, newsz);
}

AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_init_default(struct am_alloc *alloc)
{
    alloc->fn = alloc_default;
}

/****************************************************************************/

static void *obstack_chunk_alloc(size_t sz)
{
    void *data = malloc(sz);
    if (!data) {
        abort();
    }
    return data;
}
static void obstack_chunk_free(void *data)
{
    free(data);
}
static void failed(void)
{
    exit(EXIT_FAILURE);
}

static
void *alloc_stack(void *ptr, size_t oldsz, size_t newsz, struct am_alloc *_self)
{
    struct am_alloc_stack *self = (struct am_alloc_stack *)_self;

    if (ptr == NULL) {
        return obstack_alloc(&self->obstack, newsz);
    } else if (newsz == 0) {
        obstack_free(&self->obstack, ptr);
    } else {
        /* Note: this doesn't free the previously used memory! */
        void *p;
        p = obstack_alloc(&self->obstack, newsz);
        memcpy(p, ptr, oldsz);
        return p;
    }
}

AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_init_stack(struct am_alloc_stack *alloc)
{
    obstack_alloc_failed_handler = failed;
    alloc->alloc.fn = alloc_stack;
    obstack_init(&alloc->obstack);
}

AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_destroy_stack(struct am_alloc_stack *alloc)
{
    alloc->alloc.fn = NULL;
    obstack_free(&alloc->obstack, NULL);
}
