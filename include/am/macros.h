/** @file am/macros.h
 * @brief Generally useful macro definitions
 */

#ifndef AM_MACROS_H
#define AM_MACROS_H 1

#include <stddef.h>

/* Utilities */

/** @brief Convert a token into a string
 * @param x A token or macro to expand before converting to a string
 */
#define AM_STRINGIFY(x)  AM__STRINGIFY(x)
#define AM__STRINGIFY(x) #x
/** @brief Concatenate two arguments */
#define AM_CONCAT(a, b)  AM__CONCAT(a, b)
#define AM__CONCAT(a, b) a##b
/** @brief A non-macro pragma
 * @param val A token stream representing the pragma
 */
#define AM_PRAGMA(val) _Pragma(#val)
/** @brief Compile-time assertion */
#define AM_STATIC_ASSERT(expr, message) __extension__ _Static_assert(expr, message)
/** @brief String literal representing the current location */
#define AM_STRLOC __FILE__ ":" AM_STRINGIFY(__LINE__)
/** @brief Null-terminated string representing the current function */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define AM_STRFUNC ((const char *)(__func__))
#else
#  define AM_STRFUNC ((const char *)("???"))
#endif
/** @brief Get a pointer to the containing structure
 * @param ptr A pointer to the structure field
 * @param type The type of the containing structure
 * @param member The name of the structure member
 */
#define AM_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
/** @brief The alignment of the given type
 * @note Try not to provide function types unlessed typedef-ed
 */
#define AM_ALIGNOF_TYPE(type) __extension__ _Alignof(type)

/* Keywords */

/** @brief Portable `inline` */
#define AM_INLINE __inline
/** @brief Portable `restrict` */
#define AM_RESTRICT __restrict__

/* Attributes */

/** @brief Decorator for a function who's result matters */
#define AM_ATTR_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
/** @brief Decorator for a vararg function who's last argument must be NULL */
#define AM_ATTR_SENTINEL(pos) __attribute__((__sentinel__(pos)))
/** @brief Decorator for a function that doesn't return */
#define AM_NORETURN __attribute__((__noreturn__))
/** @brief Decorator for a function that requires non-null arguments */
#define AM_ATTR_NON_NULL(args) __attribute__((__nonnull__ args))
/** @brief Decorator for a printf-style function
 * @param str_idx The index of the string argument
 * @param first_to_check The first index to check for varargs
 */
#define AM_ATTR_PRINTF(str_idx, first_to_check) __attribute__((__format__(__printf__, str_idx, first_to_check)))
/** @brief Decorator for a function that returns non-aliasing pointers */
#define AM_ATTR_MALLOC __attribute__((__malloc__))
/** @brief Decorator for a function who's output only depends on inputs + globals */
#define AM_ATTR_PURE __attribute__((__pure__))
/** @brief Decorator for a function who's output only depends on inputs */
#define AM_ATTR_CONST __attribute__((__const__))
/** @brief Decorator for a function that should always be inlined */
#define AM_ATTR_ALWAYS_INLINE __attribute__((__always_inline__)) AM_INLINE
/** @brief Decorator for a function that should never be inlined */
#define AM_ATTR_NEVER_INLINE __attribute__((__noinline__))
/** @brief Decorator for a function returns a non-null pointer */
#define AM_ATTR_RETURNS_NON_NULL

/** @brief Decorator for a fallthrough switch case */
#define AM_ATTR_FALLTHROUGH __attribute__((__fallthrough__))

/* Visibility */

#define AM_IMPORT
#define AM_PUBLIC  __attribute__((__visibility__("default")))
#define AM_PRIVATE __attribute__((__visibility__("hidden")))

/* Math */

#define AM_MAX(a, b) ((a) > (b) ? (a) : (b))
#define AM_MIN(a, b) ((a) < (b) ? (a) : (b))
#define AM_ABS(a)    ((a) < 0  ? -(a) : (b))
#define AM_CLAMP(x, high, low) AM_MAX(low, (AM_MIN(x, high)))
#define AM_APPROX_EQUAL(a, b, epsilon) (AM_ABS((a) - (b)) < (epsilon))

/* Misc */

#define AM_LIKELY(x)   (__builtin_expect(!!(x), 1))
#define AM_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#define AM_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
/** @brief Whether the compiler supports variadic macros
 * 0 - No variadic macros supported
 * 1 - C99 variadic macros supported: \#define(...)    f(__VA_ARGS__)
 * 2 - GNU variadic macros supported: \#define(fmt...) f(fmt)
 */
#define AM_HAS_VARIADIC_MACROS 1
#define AM_HAS_GLIBC_PROGNAME  1
#define AM_HAS_BUILTIN_POPCOUNT 1

/* Ignoring errors/warnings */

#define AM_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#define AM_DIAGNOSTIC_POP  _Pragma("GCC diagnostic pop")
#define AM_DIAGNOSTIC_IGNORE_VARIADIC_MACROS _Pragma("GCC diagnostic ignored \"-Wvariadic-macros\"")
#define AM_DIAGNOSTIC_IGNORE_CAST_FUNC_TYPE  _Pragma("GCC diagnostic ignored \"-Wcast-function-type\"")

#endif /* ifndef AM_MACROS_H */
