

#ifndef AM_MACROS_MSVC_H
#define AM_MACROS_MSVC_H 1

#include <stddef.h>
#include <sal.h>

#define AM_PRAGMA(p) __pragma(p)

#define AM_STATIC_ASSERT(expr, message) \
    typedef char AM_CONCAT(am_static_assertion_, __COUNTER__)[(e) ? 1 : -1]

#define AM_STRLOC __FILE__ ":" AM_STRINGFY(__LINE__)
#define AM_STRFUNC ((const char *)(__FUNCTION__))

#define AM_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define AM_INLINE static __inline

#define AM_RESTRICT __restrict

#define AM_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))

/* Attributes */

#define AM_LIKELY(x)   (!!(x))
#define AM_UNLIKELY(x) (!!(x))
#define AM_ASSUME(x)   __assume(x)

#define AM_ATTR_WARN_UNUSED_RESULT _Check_return_
#define AM_ATTR_SENTINEL(pos)
#define AM_ATTR_NORETURN           __declspec(noreturn)
#define AM_ATTR_NON_NULL
#define AM_ATTR_PRINTF
#define AM_ATTR_MALLOC             __declspec(restrict)
#define AM_ATTR_CONST
#define AM_ATTR_ALWAYS_INLINE      __forceinline
#define AM_ATTR_NEVER_INLINE       __noinline
#define AM_ATTR_RETURNS_NON_NULL   _Ret_notnull_
#if defined(_fallthrough)
#  define AM_ATTR_FALLTHROUGH      _fallthrough
#else
#  define AM_ATTR_FALLTHROUGH
#endif

/* Visibility */

#define AM_IMPORT  __declspec(dllimport)
#define AM_PUBLIC  __declspec(dllexport)
#define AM_PRIVATE

#endif /* ifndef AM_MACROS_MSVC_H */

