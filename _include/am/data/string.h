
#ifndef AM_DATA_STRING_H
#define AM_DATA_STRING_H 1

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "am/macros.h"

struct am_string;

static AM_INLINE bool am_string_init(struct am_string *str, const char *cstr, size_t len);
static AM_INLINE bool am_string_init_reserve(struct am_string *str, const char *cstr, size_t len, size_t reserve);
static AM_INLINE void am_string_free(struct am_string *str);

/* uint8_t *am_string_begin(struct am_string *str); */
/* uint8_t *am_string_end(struct am_string *str); */
/* uint8_t *am_string_rbegin(struct am_string *str); */
/* uint8_t *am_string_rend(struct am_string *str); */

/* #define AM_STRING_MAX_CAP */
/* char *am_string_data(struct am_string *str); */
static AM_INLINE size_t am_string_size(const struct am_string *str);
static AM_INLINE size_t am_string_capacity(const struct am_string *str);
static AM_INLINE bool am_string_reserve(struct am_string *str, size_t sz);
static AM_INLINE void am_string_clear(struct am_string *str);
static AM_INLINE bool am_string_empty(const struct am_string *str);
static AM_INLINE void am_string_shrink_to_fit(struct am_string *str);

/* uint8_t& am_string_at(struct am_string *str, size_t idx); */

static AM_INLINE bool am_string_append(struct am_string *str, const char *s, size_t sz);
static AM_INLINE bool am_string_assign(struct am_string *str, const char *s, size_t sz);
static AM_INLINE bool am_string_insert(struct am_string *str, size_t pos, const char *s, size_t sz);

/* char *am_string_data(struct am_string *str); */

/*****************************************************************************
 *
 * Implementation
 *
 * Do not danger into these depths...
 *
 *****************************************************************************/

#define AM__STRING_MIN_CAP (sizeof(void *) * 3 - 1)
#define AM_STRING_MAX_CAP AM_MIN(SIZE_MAX, UINTPTR_MAX)

struct am_string {
    union {
        /* Strings too large for small-string optimization */
        struct {
            char *_data; /* The allocated string */
            uintptr_t _size, _cap; /* Length of string + allocated capacity */
        } _long;
        /* Small string optimization */
        struct {
            char _str[AM__STRING_MIN_CAP]; /* Small string */
            uint8_t _size; /* Length of the small string */
        } _short;
    } _x;
};
AM_STATIC_ASSERT(sizeof(struct am_string) == sizeof(void *) * 3, "");

/*** Helpers ***/
#if defined(BIG_ENDIAN)
#  define AM__STRING_SHORT_MASK 0x1
#  define AM__STRING_LONG_MASK  0x1
#  define am__string_get_short_size(s)     ((s)->_x._short._size >> 1)
#  define am__string_set_short_size(s, sz) ((s)->_x._short._size = (sz) << 1)
#else /* defined(BIG_ENDIAN) */
#  define AM__STRING_SHORT_MASK 0x80
#  define AM__STRING_LONG_MASK  (~((uintptr_t)(~0) >> 1))
#  define am__string_get_short_size(s)     ((s)->_x._short._size)
#  define am__string_set_short_size(s, sz) ((s)->_x._short._size = (sz))
#endif /* defined(BIG_ENDIAN) */
#define am__string_is_short(s)          (!((s)->_x._short._size & AM__STRING_SHORT_MASK))
#define am__string_get_short_ptr(s)     ((s)->_x._short._str)
#define am__string_get_long_cap(s)      ((s)->_x._long._cap & (uintptr_t)~AM__STRING_LONG_MASK)
#define am__string_set_long_cap(s, cap) ((s)->_x._long._cap = AM__STRING_LONG_MASK | (cap))
#define am__string_get_long_size(s)     ((s)->_x._long._size)
#define am__string_set_long_size(s, sz) ((s)->_x._long._size = (sz))
#define am__string_get_long_ptr(s)      ((s)->_x._long._data)
#define am__string_set_long_ptr(s, p)   ((s)->_x._long._data = (p))

#define am__string_set_size(s, sz)      \
    (am__string_is_short(s)             \
     ? am__string_set_short_size(s, sz) \
     : am__string_set_long_size(s, sz))

static AM_INLINE uintptr_t am__string_nextcap(uintptr_t cap)
{
    if (cap < AM__STRING_MIN_CAP) {
        return AM__STRING_MIN_CAP - 1;
    } else {
        uintptr_t guess = (cap + 16) & ~15;
        if (guess == AM__STRING_MIN_CAP)
            guess++;
        return guess;
    }
}

/****************************************************************************/

static AM_INLINE bool am_string_init(struct am_string *str, const char *cstr, size_t len)
{
    char *p;
    if (len > AM_STRING_MAX_CAP) {
        return false;
    }
    if (len < AM__STRING_MIN_CAP) {
        am__string_set_short_size(str, len);
        p = am__string_get_short_ptr(str);
    } else {
        uintptr_t cap = am__string_nextcap(len);
        p = malloc(cap + 1);
        if (p == NULL) {
            return false;
        }
        am__string_set_long_ptr(str, p);
        am__string_set_long_cap(str, cap + 1);
        am__string_set_long_size(str, len);
    }
    memcpy(p, cstr, len);
    p[len] = '\0';
}

static AM_INLINE bool am_string_init_reserve(
        struct am_string *str, const char *cstr, size_t len, size_t reserve)
{
    char *p;
    if (reserve > AM_STRING_MAX_CAP) {
        return false;
    }
    if (len < AM__STRING_MIN_CAP) {
        am__string_set_short_size(str, len);
        p = am__string_get_short_ptr(str);
    } else {
        uintptr_t cap = am__string_nextcap(reserve);
        p = malloc(cap + 1);
        if (p == NULL)
            return false;
        am__string_set_long_ptr(str, p);
        am__string_set_long_cap(str, cap + 1);
        am__string_set_long_size(str, len);
    }
    memcpy(p, cstr, len);
    p[len] = '\0';
    return true;
}

static AM_INLINE void am_string_free(struct am_string *str)
{
    if (str) {
        if (!am__string_is_short(str)) {
            free(am__string_get_long_ptr(str));
        }
        memset(str, 0x0, sizeof *str);
    }
}

#define am_string_begin(str)                                              \
    (am__string_is_short(str)                                             \
     ? am__string_get_short_ptr(str)                                      \
     : am__string_get_long_ptr(str))
#define am_string_end(str)                                                \
    (am__string_is_short(str)                                             \
     ? am__string_get_short_ptr(str) + am__string_get_short_size(str)     \
     : am__string_get_long_ptr(str) + am__string_get_long_size(str))
#define am_string_rbegin(str)                                             \
    (am__string_is_short(str)                                             \
     ? am__string_get_short_ptr(str) + am__string_get_short_size(str) - 1 \
     : am__string_get_long_ptr(str) + am__string_get_long_size(str) - 1)
#define am_string_rend(str)                                               \
    (am__string_is_short(str)                                             \
     ? am__string_get_short_ptr(str) - 1                                  \
     : am__string_get_long_ptr(str) - 1)

#define am_string_data(s)         \
    (am__string_is_short(s)        \
     ? am__string_get_short_ptr(s) \
     : am__string_get_long_ptr(s))

#define am_string_at(s, i) (am_string_data(s)[i])

static AM_INLINE size_t am_string_size(const struct am_string *str)
{
    if (am__string_is_short(str)) {
        return am__string_get_short_size(str);
    } else {
        return am__string_get_long_size(str);
    }
}

static AM_INLINE size_t am_string_capacity(const struct am_string *str)
{
    if (am__string_is_short(str)) {
        return AM__STRING_MIN_CAP - 1;
    } else {
        return am__string_get_long_cap(str);
    }
}

static AM_INLINE bool am_string_reserve(struct am_string *str, size_t reserve)
{
    uintptr_t cap;
    uintptr_t size;
    if (reserve > AM_STRING_MAX_CAP)
        return false;
    cap = am_string_capacity(str);
    size = am_string_size(str);
    reserve = am__string_nextcap(AM_MAX(reserve, size));
    if (reserve != cap) {
        if (reserve == AM__STRING_MIN_CAP - 1) {
            char *new_data, *p;
            /* Since reserve != cap, we know we weren't long */
            new_data = am__string_get_short_ptr(str);
            p = am__string_get_long_ptr(str);
            memcpy(new_data, p, size + 1);
            free(p);
            am__string_set_short_size(str, size);
        } else if (am__string_is_short(str)) {
            /* Short -> Long */
            char *p;
            p = malloc(reserve + 1);
            if (p == NULL)
                return false;
            memcpy(p, am__string_get_short_ptr(str), size + 1);
            am__string_set_long_ptr(str, p);
            am__string_set_long_size(str, size);
            am__string_set_long_cap(str, reserve + 1);
        } else {
            /* Long -> Long */
            char *p;
            p = realloc(am__string_get_long_ptr(str), reserve + 1);
            if (p == NULL)
                return false;
            am__string_set_long_ptr(str, p);
            am__string_set_long_cap(str, reserve + 1);
        } 
    }
    return true;
}

static AM_INLINE void am_string_clear(struct am_string *str)
{
    if (am__string_is_short(str)) {
        am__string_get_short_ptr(str)[0] = '\0';
        am__string_set_short_size(str, 0);
    } else {
        am__string_get_long_ptr(str)[0] = '\0';
        am__string_set_long_size(str, 0);
    }
}

static AM_INLINE bool am_string_empty(const struct am_string *str)
{
    return am_string_size(str) == 0;
}

static AM_INLINE void am_string_shrink_to_fit(struct am_string *str)
{
    am_string_reserve(str, 0);
}

static AM_INLINE bool am_string_append(
        struct am_string *str, const char *s, size_t n)
{
    uintptr_t size;
    bool ok;
    char *p;

    size = am_string_size(str);
    ok = am_string_reserve(str, size + n);
    if (!ok)
        return false;
    p = am_string_data(str);
    memcpy(p + size, s, n);
    size += n;
    am__string_set_size(str, size);
    p[size] = '\0';
    return true;
}

static AM_INLINE bool am_string_assign(struct am_string *str, const char *s, size_t sz)
{
    bool ok;
    char *p;
    am__string_set_size(str, 0);
    ok = am_string_reserve(str, sz);
    if (!ok)
        return false;
    am__string_set_size(str, sz);
    p = am_string_data(str);
    memcpy(p, s, sz);
    p[sz] = '\0';
    return true;
}

static AM_INLINE bool am_string_insert(struct am_string *str, size_t pos, const char *s, size_t sz)
{
    (void)str; (void)pos; (void)s; (void)sz;
    /* uintptr_t size, cap; */
    /* char *p; */
    assert(0);
    /* size = am_string_size(str); */
    /* if (sz > size) */
        /* return false; */
    /* cap = am_string_capacity(str); */
    /* am_string_reserve(str, size + sz); */
    /* p = am_string_data(str); */
    /* memmove() */
}

#endif /* ifndef AM_DATA_STRING_H */
