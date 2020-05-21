
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <immintrin.h>

/* Bitmask */
typedef uint32_t bitmask;

#define bitmask_foreach(it, bitmask) \
    for ((it) = (bitmask); (it); (it) &= ((it) - 1))

#define bitmask_iterator_to_idx(it) __builtin_ctz(it)

#define H1(hash) ((hash) >> 7)
#define H2(hash) ((hash) & 0x7F)

#define ctrl_empty    (-128)
#define ctrl_deleted  (-2)
#define ctrl_sentinel (-1)

/* Group */
struct group_meta {
    __m128i ctrl;
};

bitmask group_matches(const struct group_meta *g, int8_t h2_hash) {
    return _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_set1_epi8(h2_hash), g->ctrl));
}
bool group_is_full(const struct group_meta *g) {
    return (bool)_mm_movemask_epi8(_mm_sign_epi8(g->ctrl, g->ctrl));
}
static const struct group_meta *empty_group(void) {
    static const int8_t g[] = {
        ctrl_sentinel, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
    };
    return (struct group_meta *)g;
}

/* #define T int

struct hashtable {
    size_t size;
    struct group_meta *meta;
    T *slots;
};

struct iterator {
    struct group_meta *meta;
    T *slot;
};

struct iterator hashtable_find(const struct hashtable *ht, T key, size_t hash) {
    size_t group = H1(hash) % ht->size;
    while (true) {
        struct group_meta *g = &ht->meta[group];
        uint32_t it;
        bitmask_foreach(it, group_matches(g, H2(hash))) {
            size_t idx = bitmask_iterator_to_idx(it);
            if (key == ht->slots[idx]) {
                struct iterator r;
                r.meta = &ht->meta[idx];
                r.slot = &ht->slots[idx];
                return r;
            }
        }
        if (group_is_full(g)) {
            struct iterator r = { NULL, NULL };
            return r;
        }
    }
} */

/* Groups */

