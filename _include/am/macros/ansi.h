
#ifndef AM_MACROS_ANSI_H
#define AM_MACROS_ANSI_H 1

#include <stddef.h> /* offsetof */

#if AM_C99
#  define AM_PRAGMA(p) _Pragma(#p)
#else
#  define AM_PRAGMA(_)
#endif

/* Note that this requires static assertion declarations to be on separate lines */
#if AM_C11
#  define AM_STATIC_ASSERT(expr, message) _Static_assert(expr, message)
#else
#  define AM_STATIC_ASSERT(expr, message) \
    typedef char AM_CONCAT(am_static_assertion_, __LINE__)[(expr) ? 1 : -1]
#endif

#define AM_STRLOC __FILE__ ":" AM_STRINGIFY(__LINE__)
#if AM_C99
#  define AM_STRFUNC ((const char *)(__func__))
#else
#  define AM_STRFUNC ((const char *)("???"))
#endif

#define AM_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define AM_ALIGNOF_TYPE(type) offsetof(struct { char _a; type _b; }, _b)

#if AM_C99
#  define AM_INLINE static inline
#else
#  define AM_INLINE static
#endif
#if AM_C99
#  define AM_RESTRICT restrict
#else
#  define AM_RESTRICT
#endif

#define AM_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))

/* Attributes */

#define AM_LIKELY(x)   (!!(x))
#define AM_UNLIKELY(x) (!!(x))
#define AM_ASSUME(x)   ((void)0)

#define AM_ATTR_WARN_UNUSED_RESULT
#define AM_ATTR_SENTINEL(pos)
#if AM_C11
#  define AM_ATTR_NORETURN _Noreturn
#else
#  define AM_ATTR_NORETURN
#endif
#define AM_ATTR_NON_NULL
#define AM_ATTR_PRINTF
#define AM_ATTR_MALLOC
#define AM_ATTR_CONST
#define AM_ATTR_ALWAYS_INLINE
#define AM_ATTR_NEVER_INLINE
#define AM_ATTR_RETURNS_NON_NULL
#define AM_ATTR_FALLTHROUGH

/* Visibility */

#define AM_IMPORT
#define AM_PUBLIC
#define AM_PRIVATE

#endif /* ifndef AM_MACROS_ANSI_H */
