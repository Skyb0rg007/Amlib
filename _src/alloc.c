
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "am/alloc.h"
#include "am/data/vector.h"

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
        return NULL;
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

/****************************************************************************/

struct am_alloc_pool_page {
    char memory[4096];
};

union am_alloc_pool_node {
    union am_alloc_pool_node *prev;
    char chunk[CHAR_BIT]; /* flexible array member */
};

static
void init_page(struct am_alloc_pool *alloc, struct am_alloc_pool_page *page)
{
    union am_alloc_pool_node *newstack = (void *)page->memory;
    size_t num_chunks = sizeof *page / alloc->chunk_size;
    size_t i;

    newstack->prev = alloc->stack;
    for (i = 0; i < num_chunks - 1; i++) {
        union am_alloc_pool_node *nextstack = (union am_alloc_pool_node *)((char *)newstack + alloc->chunk_size);
        nextstack->prev = newstack;
        newstack = nextstack;
    }
    alloc->stack = newstack;
}

static
bool grow(struct am_alloc_pool *alloc)
{
    struct am_alloc_pool_page *page = malloc(sizeof *page);
    memset(page, 0xb, sizeof *page);
    if (page == NULL) {
        return false;
    }
    init_page(alloc, page);
    am_vector_append(&alloc->pages, page);
    return true;
}

static
void *alloc_pool(void *ptr, size_t oldsz, size_t newsz, struct am_alloc *_self)
{
    struct am_alloc_pool *self = (struct am_alloc_pool *)_self;
    (void)oldsz;

    if (ptr == NULL) {
        /* malloc */
        union am_alloc_pool_node *node;
        assert(oldsz == 0);
        assert(newsz <= self->chunk_size);
        if (self->stack == NULL) {
            if (!grow(self)) {
                return NULL;
            }
        }
        node = self->stack;
        self->stack = self->stack->prev;
        return node->chunk;
    } else if (newsz == 0) {
        /* free */
        union am_alloc_pool_node *node;
        assert(oldsz == self->chunk_size);
        node = ptr;
        node->prev = self->stack;
        self->stack = ptr;
        return NULL;
    } else if (oldsz <= self->chunk_size && newsz <= self->chunk_size) {
        /* No shinking/growing */
        return ptr;
    } else {
        /* realloc not implemented */
        return NULL;
    }
}

AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_init_pool(struct am_alloc_pool *alloc, size_t chunk_size)
{
    alloc->alloc.fn = alloc_pool;
    am_vector_init(&alloc->pages);
    alloc->stack = NULL;
    alloc->chunk_size = chunk_size;
}

AM_PUBLIC AM_ATTR_NON_NULL((1))
void am_alloc_destroy_pool(struct am_alloc_pool *alloc)
{
    size_t i;
    for (i = 0; i < alloc->pages.size; i++) {
        free(am_vector_at(&alloc->pages, i));
    }
    am_vector_destroy(&alloc->pages);
    memset(alloc, 0x0, sizeof *alloc);
}
