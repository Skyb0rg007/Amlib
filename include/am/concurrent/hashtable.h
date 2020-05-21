
#ifndef AM_LOCKFREE_HASHTABLE_H
#define AM_LOCKFREE_HASHTABLE_H 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "am/macros.h"
#include "am/atomic.h"
#include "am/alloc.h"

#error NYI

enum am_hashtable_mode {
    AM_HASHTABLE_MODE_DIRECT,
    AM_HASHTABLE_MODE_BYTESTRING
};

struct am_hashtable_entry {
    uintptr_t key;
    uintptr_t value;
    uintptr_t key_length;
    uintptr_t hash;
} /* __attribute__((aligned(32))) */;

#define AM_HASHTABLE_KEY_EMPTY     ((uintptr_t)0)
#define AM_HASHTABLE_KEY_TOMBSTONE (~((uintptr_t)0))

typedef uint64_t am_hashtable_hash_fn(const void *data, size_t len, uint64_t seed);

struct am_hashtable_map;
struct am_hashtable {
    struct am_alloc *allocator;
    struct am_hashtable_map *map;
    unsigned mode;
    uint64_t seed;
    am_hashtable_hash_fn *hash_fn;
};

struct am_hashtable_stat {
    uint64_t probe_maximum;
    uint64_t num_entries;
};

struct am_hashtable_iterator {
    struct am_hashtable_entry *current;
    uint64_t offset;
};

#define AM_HASHTABLE_ITERATOR_INITIALIZER { NULL, 0 }

AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hashtable_iterator_init(struct am_hashtable_iterator *it)
{
    it->current = NULL;
    it->offset = 0;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
bool am_hashtable_entry_empty(const struct am_hashtable_entry *entry)
{
    return entry->key == AM_HASHTABLE_KEY_EMPTY;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hashtable_entry_key_set_direct(struct am_hashtable_entry *entry, uintptr_t key)
{
    entry->key = key;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hashtable_entry_key_set(struct am_hashtable_entry *entry, void *key, uint16_t key_length)
{
    entry->key = (uintptr_t)key;
    entry->key_length = key_length;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
uint16_t am_hashtable_entry_key_length(const struct am_hashtable_entry *entry)
{
    return entry->key_length;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
void *am_hashtable_entry_value(struct am_hashtable_entry *entry)
{
    return (void *)entry->value;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
uintptr_t am_hashtable_entry_value_direct(struct am_hashtable_entry *entry)
{
    return entry->value;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
void *am_hashtable_entry_key(struct am_hashtable_entry *entry)
{
    return (void *)entry->key;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
uintptr_t am_hashtable_entry_key_direct(struct am_hashtable_entry *entry)
{
    return entry->key;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hashtable_entry_set(
        struct am_hashtable_entry *entry,
        uint64_t hash,
        const void *key,
        uint16_t key_length,
        const void *value)
{
    entry->key        = (uintptr_t)key;
    entry->value      = (uintptr_t)value;
    entry->key_length = key_length;
    entry->hash       = hash;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_hashtable_entry_set_direct(
        struct am_hashtable_entry *entry,
        uint64_t hash,
        uintptr_t key,
        uintptr_t value)
{
    entry->key   = key;
    entry->value = value;
    entry->hash  = hash;
}

#endif /* ifndef AM_LOCKFREE_HASHTABLE_H */
