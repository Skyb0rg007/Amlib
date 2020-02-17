
#ifndef AM_DATA_HASH_H
#define AM_DATA_HASH_H 1

#include <stdint.h>
#include <limits.h>
#include "am/macros.h"

static AM_ATTR_ALWAYS_INLINE
uint32_t am_hash_rotl32(uint32_t val, uint8_t count)
{
    const uint32_t mask = CHAR_BIT * sizeof(val) - 1;
    count &= mask;
    return (val << count) | (val >> (-count & mask));
}

static AM_ATTR_ALWAYS_INLINE
uint32_t am_hash_rotl64(uint64_t val, uint8_t count)
{
    const uint32_t mask = CHAR_BIT * sizeof(val) - 1;
    count &= mask;
    return (val << count) | (val >> (-count & mask));
}

static AM_ATTR_ALWAYS_INLINE
uint32_t am_hash_fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= UINT32_C(0x85ebca6b);
    h ^= h >> 13;
    h *= UINT32_C(0xc2b2ae35);
    h ^= h >> 16;

    return h;
}

static AM_INLINE
uint64_t am_hash_fmix64(uint64_t h)
{
    h ^= h >> 33;
    h *= UINT64_C(0xff51afd7ed558ccd);
    h ^= h >> 33;
    h *= UINT64_C(0xc4ceb9fe1a85ec53);
    h ^= h >> 33;
    return h;
}

static AM_INLINE
uint32_t am_hash_murmur_x86_32(const void *key, unsigned len, uint32_t seed)
{
    const uint8_t *data = key;
    const int nblocks = len / 4;

    uint32_t h1 = seed;
    uint32_t k1;
    int i;

    const uint32_t c1 = UINT32_C(0xcc9e2d51);
    const uint32_t c2 = UINT32_C(0x1b873593);

    const uint32_t *blocks;
    const uint8_t *tail;

    /* body */
    blocks = (const uint32_t *)(data + nblocks * 4);

    for(i = -nblocks; i; i++)
    {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = am_hash_rotl32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = am_hash_rotl32(h1, 13); 
        h1 = h1 * 5 + UINT32_C(0xe6546b64);
    }

    /* tail */
    tail = (const uint8_t*)(data + nblocks * 4);

    k1 = 0;

    switch(len & 3)
    {
        case 3:
            k1 ^= tail[2] << 16;
            AM_ATTR_FALLTHROUGH; /* fall through */
        case 2:
            k1 ^= tail[1] << 8;
            AM_ATTR_FALLTHROUGH; /* fall through */
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = am_hash_rotl32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    };

    /* finalization */
    h1 ^= len;
    h1 = am_hash_fmix32(h1);
    return h1;
}

static AM_INLINE
uint32_t am_hash_fnva1_32(const void *key, unsigned len)
{
    const uint32_t offset = UINT32_C(0x811c9dc5);
    const uint32_t prime = UINT32_C(0x01000193);

    const uint8_t *data = key;
    uint32_t hash;
    unsigned i;

    hash = offset;
    for (i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= prime;
    }
    return hash;
}

static AM_INLINE
uint64_t am_hash_fnva1_64(const void *key, unsigned len)
{
    const uint64_t offset = UINT64_C(0xcbf29ce484222325);
    const uint64_t prime = UINT64_C(0x00000100000001B3);

    const uint8_t *data = key;
    uint64_t hash;
    unsigned i;

    hash = offset;
    for (i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= prime;
    }
    return hash;
}

#endif /* ifndef AM_DATA_HASH_H */
