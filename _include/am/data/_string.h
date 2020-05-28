
#ifndef AM_DATA_STRING_H
#define AM_DATA_STRING_H 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "am/macros.h"
#include "am/alloc.h"

#define am_basic_string(c)                                    \
    struct {                                                  \
        union {                                               \
            struct {                                          \
                c *_ptr;                                      \
                uintptr_t _size;                              \
                uintptr_t _capacity;                          \
            } _long;                                          \
            struct {                                          \
                c _str[(3 * sizeof(void *)) / sizeof(c) - 1]; \
                uint8_t _size;                                \
            } _short;                                         \
        } _data;                                              \
    }

#define am_basic_string_min_capacity(s) \
    ((3 * sizeof(void *)) / sizeof((s)->_data._long._ptr[0]) - 1)

#define am__basic_string_set_short_size(s, len)

/* bool am_basic_string_init(am_basic_string(T) *str, T *cstr, size_t len) */
typedef am_basic_string(char) am_string;
static AM_INLINE bool am_basic_string_init_alloc(am_string *str, char *cstr, size_t len, am_alloc_fn *f, void *ud)
{
    if (len > am_basic_string_min_capacity(str)) {
        memcpy(str->_data._short._str, cstr, len);
        str->_data._short._str[len] = 0;
        am__basic_string_set_short_size(str, len);
        return true;
    } else {
        char *p;
        p = am_malloc(f, ud, len + 1);
        if (!p)
            return false;
        str->_data._long._ptr = p;
        memcpy(p, cstr, len);
        p[len] = 0;
    }
}
#define am_basic_string_init(str, cstr, len)         \
    ((len) > am_basic_string_min_capacity(str)       \
     ? (memcpy((str)->_data._short._str, cstr, len), \
        (str)->_data._short._str[len] = 0,           \
        am__basic_string_set_short_size(str, len))   \
     : ())

#endif /* ifndef AM_DATA_STRING_H */
