/** @file am/common.h
 * @brief Common macros and definitions
 */

#ifndef AM_COMMON_H
#define AM_COMMON_H 1

/* Include all C99 freestanding headers
 * These headers are really easy to implement oneself,
 * so we assume they are available
 */
#include <float.h>
#include <iso646.h>  /* C95 */
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h> /* C99 */
#include <stddef.h>
#include <stdint.h>  /* C99 */

/*****************************************************************************
 *
 * Miscellaneous Macros
 *
 ****************************************************************************/

/** @def AM_STRINGIFY
 * @hideinitializer
 * @brief Convert a symbol into a string, expanding macros
 */
#define AM_STRINGIFY(x) AM__STRINGIFY(x)
/** @cond */
#define AM__STRINGIFY(x) #x
/** @endcond */

/** @def AM_CONCAT
 * @hideinitializer
 * @brief Concatenate two symbols, expanding macros
 */
#define AM_CONCAT(a, b) AM__CONCAT(a, b)
/** @cond */
#define AM__CONCAT(a, b) a##b
/** @endcond */

/** @brief Numerical maximum @note Evaluates arguments more than once */
#define AM_MAX(a, b) ((a) > (b) ? (a) : (b))
/** @brief Numerical minimum @note Evaluates arguments more than once */
#define AM_MIN(a, b) ((a) < (b) ? (a) : (b))
/** @brief Absolute value @note Evaluates argument more than once */
#define AM_ABS(a) ((a) < 0 ? -(a) : (a))
/** @brief Clamp a number between two values @note Evaluates arguments more than once */
#define AM_CLAMP(x, low, high) (AM_MAX(low, (AM_MIN(x, high))))
/** @brief Determine if two numbers are within @p epsilon of another
 * @note Evaluates @p a and @p b more than once
 * This is meant for testing floating-point equality
 */
#define AM_APPROX_EQUAL(a, b, epsilon) (AM_ABS((a) - (b)) < (epsilon))

/** @brief Access the parent structure given an internal member
 * @param ptr Pointer to the structure member
 * @param type The type of the resulting structure
 * @param member The name of the struct member that @p ptr is
 * @return A pointer to the parent structure
 */
#define AM_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/** @def AM_ALIGNOF_TYPE
 * @brief Determine the alignment of a given type
 * @note The type must not be a function/array type, typedef beforehand if so
 */
#define AM_ALIGNOF_TYPE(type) offsetof(struct { char _a; type _b; }, _b)

/*****************************************************************************
 *
 * Versioning
 *
 ****************************************************************************/

/** @brief Convert major.minor.patch version into a comparable number */
#define AM_VERSION_ENCODE(major, minor, patch) (((major) * 1000000) + ((minor) * 1000) + (patch))
/** @brief Extract the major version */
#define AM_VERSION_DECODE_MAJOR(vsn) ((vsn) / 1000000)
/** @brief Extract the minor version */
#define AM_VERSION_DECODE_MINOR(vsn) (((vsn) % 1000000) / 1000)
/** @brief Extract the patch version */
#define AM_VERSION_DECODE_PATCH(vsn) ((vsn) % 1000)

/*****************************************************************************
 *
 * Version detection
 *
 *****************************************************************************/

/** @def AM_STDC_C89
 * @hideinitializer
 * @brief Synonym for @ref AM_STDC_C90
 */
/** @def AM_STDC_C90
 * @hideinitializer
 * @brief True if the compiler supports C90
 */
/** @def AM_STDC_C94
 * @hideinitializer
 * @brief Synonym for @ref AM_STDC_C95
 */
/** @def AM_STDC_C95
 * @hideinitializer
 * @brief True if the compiler supports C95
 */
/** @def AM_STDC_C99
 * @hideinitializer
 * @brief True if the compiler supports C99
 */
/** @def AM_STDC_C11
 * @hideinitializer
 * @brief True if the compiler supports C11
 */
/** @def AM_STDC_C17
 * @hideinitializer
 * @brief True if the compiler supports C17
 */
/** @def AM_VERSION_CHECK_STDC
 * @hideinitializer
 * @brief Determine if compiling under a given version of the C standard
 * @param vsn
 * @return True if the compiler supports the given version of C
 * @note @p vsn must be one of: 89, 90, 94, 95, 99, 11, 17
 */
#ifndef __STDC_VERSION__
#  ifndef __STDC__
#    define AM_STDC_C89 0
#    define AM_STDC_C90 0
#  else
#    define AM_STDC_C89 1
#    define AM_STDC_C90 1
#  endif
#  define AM_STDC_C94 0
#  define AM_STDC_C95 0
#  define AM_STDC_C99 0
#  define AM_STDC_C11 0
#  define AM_STDC_C17 0
#else
#  define AM_STDC_C89 1
#  define AM_STDC_C90 1
#  define AM_STDC_C94 (__STDC_VERSION__ >= 199409L)
#  define AM_STDC_C95 (__STDC_VERSION__ >= 199409L)
#  define AM_STDC_C99 (__STDC_VERSION__ >= 199901L)
#  define AM_STDC_C11 (__STDC_VERSION__ >= 201112L)
#  define AM_STDC_C17 (__STDC_VERSION__ >= 201710L)
#endif
#define AM_VERSION_CHECK_STDC(vsn) AM_STDC_C##vsn

/** @def AM_VERSION_CHECK_GNUC
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of GNUC
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given GNUC version
 * @note This does not check for gcc, only a compiler that defines __GNUC__
 */
#ifndef __GNUC__
#  define AM_VERSION_CHECK_GNUC(major, minor, patch) (0)
#else
#  ifdef __GNUC_PATCHLEVEL__
#    define AM_VERSION_GNUC AM_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#  else
#    define AM_VERSION_GNUC AM_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, 0)
#  endif
#  define AM_VERSION_CHECK_GNUC(major, minor, patch) (AM_VERSION_GNUC >= AM_VERSION_ENCODE(major, minor, patch))
#endif

/** @def AM_VERSION_CHECK_MSVC
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of MSVC
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given MSVC version
 */
#ifndef _MSC_VER
#  define AM_VERSION_CHECK_MSVC(major, minor, patch) (0)
#else
#  if _MSC_VER >= 1400
#    define AM_VERSION_CHECK_MSVC(major, minor, patch) (_MSC_FULL_VER >= ((major * 10000000) + (minor * 100000) + (patch)))
#  elif _MSC_VER >= 1200
#    define AM_VERSION_CHECK_MSVC(major, minor, patch) (_MSC_FULL_VER >= ((major * 1000000) + (minor * 10000) + (patch)))
#  else
#    define AM_VERSION_CHECK_MSVC(major, minor, patch) (_MSC_VER >= ((major * 100) + (minor)))
#  endif
#endif

/** @def AM_VERSION_CHECK_INTEL
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of ICC
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given Intel version
 */
#ifndef __INTEL_COMPILER
#  define AM_VERSION_CHECK_INTEL(major, minor, patch) (0)
#else
#  ifdef __INTEL_COMPILER_UPDATE
#    define AM_VERSION_INTEL(major, minor, patch) AM_VERSION_ENCODE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE)
#  else
#    define AM_VERSION_INTEL(major, minor, patch) AM_VERSION_ENCODE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, 0)
#  endif
#  define AM_VERSION_CHECK_INTEL(major, minor, patch) (AM_VERSION_INTEL >= AM_VERSION_ENCODE(major, minor, patch))
#endif

/** @def AM_VERSION_CHECK_PGI
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of PGI
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given PGI version
 */
#if !defined(__PGI) || !defined(__PGIC__) || !defined(__PGIC_MINOR__) || !defined(__PGIC_PATCHLEVEL__)
#  define AM_VERSION_CHECK_PGI(major, minor, patch) (0)
#else
#  define AM_VERSION_PGI AM_VERSION_ENCODE(__PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__)
#  define AM_VERSION_CHECK_PGI(major, minor, patch) (AM_VERSION_PGI >= AM_VERSION_ENCODE(major, minor, patch))
#endif

/** @def AM_VERSION_CHECK_CLANG
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of Clang
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given Clang version
 * @note Don't use this to detect feature availability - use @ref AM_HAS_BUILTIN and the like
 */
#if !defined(__clang__)
#  define AM_VERSION_CHECK_CLANG(major, minor, patch) (0)
#else
#  define AM_VERSION_CLANG AM_VERSION_ENCODE(__clang_major__, __clang_minor__, __clang_patchlevel__)
#  define AM_VERSION_CHECK_CLANG(major, minor, patch) (AM_VERSION_CLANG >= AM_VERSION_ENCODE(major, minor, patch))
#endif

/** @def AM_VERSION_CHECK_EMSCRIPTEN
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of Emscripten
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given Emscripten version
 */
#ifndef __EMSCRIPTEN__
#  define AM_VERSION_CHECK_EMSCRIPTEN(major, minor, patch) (0)
#else
#  define AM_VERSION_EMSCRIPTEN AM_VERSION_ENCODE(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#  define AM_VERSION_CHECK_EMSCRIPTEN(major, minor, patch) (AM_VERSION_EMSCRIPTEN >= AM_VERSION_ENCODE(major, minor, patch))
#endif

/** @def AM_VERSION_CHECK_TINYC
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of TinyC
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given TinyC version
 */
#ifndef __TINYC__
#  define AM_VERSION_CHECK_TINYC(major, minor, patch) (0)
#else
#  define AM_VERSION_TINYC AM_VERSION_ENCODE(__TINYC__ / 1000, (__TINYC__ / 100) % 10, __TINYC__ % 100)
#  define AM_VERSION_CHECK_TINYC(major, minor, patch) (AM_VERSION_TINYC >= AM_VERSION_ENCODE(major, minor, patch))
#endif

/** @def AM_VERSION_CHECK_GCC
 * @hideinitializer
 * @brief Determine if compiling with at least a given version of GCC
 * @param major The major version
 * @param minor The minor version
 * @param patch The patch version
 * @return true when using a compiler with at least the given GCC version
 */
#if AM_VERSION_CHECK_GNUC(0, 0, 0) && \
    !AM_VERSION_CHECK_CLANG(0, 0, 0) && \
    !AM_VERSION_CHECK_INTEL(0, 0, 0) && \
    !AM_VERSION_CHECK_PGI(0, 0, 0)
#  define AM_VERSION_CHECK_GCC(major, minor, patch) AM_VERSION_CHECK_GNUC(major, minor, patch)
#else
#  define AM_VERSION_CHECK_GCC(major, minor, patch) (0)
#endif

/*****************************************************************************
 *
 * Clang feature test macros
 *
 *****************************************************************************/

/** @brief Determine if Clang supports a given GNU-style attribute @hideinitializer */
#ifdef __has_attribute
#  define AM_HAS_ATTRIBUTE(attr) __has_attribute(attr)
#else
#  define AM_HAS_ATTRIBUTE(attr) (0)
#endif

/** @brief Determine if Clang supports a given __builtin_* function @hideinitializer */
#ifdef __has_builtin
#  define AM_HAS_BUILTIN(attr) __has_builtin(attr)
#else
#  define AM_HAS_BUILTIN(attr) (0)
#endif

/** @brief Determine if Clang supports a given language extension @hideinitializer */
#ifdef __has_extension
#  define AM_HAS_EXTENSION(attr) __has_extension(attr)
#else
#  define AM_HAS_EXTENSION(attr) (0)
#endif

/** @brief Determine if Clang supports a given language feature @hideinitializer */
#ifdef __has_feature
#  define AM_HAS_FEATURE(attr) __has_feature(attr)
#else
#  define AM_HAS_FEATURE(attr) AM_HAS_EXTENSION(attr)
#endif

/*****************************************************************************
 *
 * Compiler Optimizations / Attributes
 *
 *****************************************************************************/

/** @def AM_LIKELY
 * @hideinitializer
 * @brief Marks the expression as likely to evaluate to true
 * @code if(AM_LIKELY(foo < 3)) { ... }@endcode
 */
/** @def AM_UNLIKELY
 * @hideinitializer
 * @brief Marks the expression as unlikely to evaluate to true
 * @code if(AM_UNLIKELY(foo >= 3)) { ... }@endcode
 */
#if AM_HAS_BUILTIN(__builtin_expect) || \
    AM_VERSION_CHECK_GCC(3, 0, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    AM_VERSION_CHECK_TINYC(0, 9, 27)
#  define AM_LIKELY(expr)   __builtin_expect(!!(expr), 1)
#  define AM_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#  define AM_LIKELY(expr)   (!!(expr))
#  define AM_UNLIKELY(expr) (!!(expr))
#endif

/** @def AM_ATTR_MALLOC
 * @hideinitializer
 * @brief Function attribute signaling `malloc`-like behavior.
 * Any non-null pointer returned by such a function does not alias with any other pointer,
 * and the data pointed to doesn't contain valid pointers. Example: `malloc`.
 */
#if AM_HAS_ATTRIBUTE(malloc) || \
    AM_VERSION_CHECK_GCC(3, 1, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_ATTR_MALLOC __attribute__((__malloc__))
#elif AM_VERSION_CHECK_MSVC(14, 0, 0)
#  define AM_ATTR_MALLOC __declspec(restrict)
#else
#  define AM_ATTR_MALLOC
#endif

/** @def AM_ATTR_PURE
 * @hideinitializer
 * @brief Function attribute signaling a pure function.
 * A pure function's return value depends only on its parameters, globals, and pointer dereferences
 * A call of a pure function can be optimized-out. Example: `strlen`.
 */
#if AM_HAS_ATTRIBUTE(pure) || \
    AM_VERSION_CHECK_GCC(2, 96, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    AM_VERSION_CHECK_PGI(17, 10, 0)
#  define AM_ATTR_PURE __attribute__((__pure__))
#else
#  define AM_ATTR_PURE
#endif
#include <math.h>

/** @def AM_ATTR_CONST
 * @hideinitializer
 * @brief Function attribute signaling a const function.
 * A const function's return value depends only on its parameters
 * A call of a const function can be optimized-out, or re-ordered.
 * Example: `round`.
 */
#if AM_HAS_ATTRIBUTE(const) || \
    AM_VERSION_CHECK_GCC(2, 5, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    AM_VERSION_CHECK_PGI(17, 10, 0)
#  define AM_ATTR_CONST __attribute__((__const__))
#else
#  define AM_ATTR_CONST AM_ATTR_PURE
#endif

/** @def AM_RESTRICT
 * @hideinitializer
 * @brief Pointer attribute signaling a non-aliasing pointer.
 * During execution of a block, if an object accessible through a restricted
 * pointer is modified, that object must be read/written exclusively via that
 * pointer.
 */
#if AM_VERSION_CHECK_STDC(99)
#  define AM_RESTRICT restrict
#elif AM_VERSION_CHECK_GCC(3, 1, 0) || \
      AM_VERSION_CHECK_INTEL(13, 0, 0) || \
      AM_VERSION_CHECK_PGI(17, 10, 0) || \
      AM_VERSION_CHECK_CLANG(0, 0, 0)
#  define AM_RESTRICT __restrict
#else
#  define AM_RESTRICT
#endif

/** @def AM_INLINE
 * @hideinitializer
 * @brief Function qualifier that suggests the compiler inline it if possible
 */
#if AM_VERSION_CHECK_STDC(99)
#  define AM_INLINE inline
#elif AM_VERSION_CHECK_GCC(0, 0, 0)
#  define AM_INLINE __inline__
#elif AM_VERSION_CHECK_MSVC(12, 0, 0)
#  define AM_INLINE __inline
#else
#  define AM_INLINE
#endif

/** @def AM_ALWAYS_INLINE
 * @hideinitializer
 * @brief Function qualifier that suggests the compiler inline it if possible
 * This qualifier is more aggressive that @ref AM_INLINE, but does not always 
 * guarentee inlining.
 */
#if AM_HAS_ATTRIBUTE(always_inline) || \
    AM_VERSION_CHECK_GCC(4, 0, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_ALWAYS_INLINE __attribute__((__always_inline__)) AM_INLINE
#elif AM_VERSION_CHECK_MSVC(12, 0, 0)
#  define AM_ALWAYS_INLINE __forceinline
#else
#  define AM_ALWAYS_INLINE AM_INLINE
#endif

/** @def AM_NEVER_INLINE
 * @hideinitializer
 * @brief Function qualifier that suggests the compiler not inline it
 */
#if AM_HAS_ATTRIBUTE(noinline) || \
    AM_VERSION_CHECK_GCC(4, 0, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_NEVER_INLINE __attribute__((__noinline__))
#elif AM_VERSION_CHECK_MSVC(13, 10, 0)
#  define AM_NEVER_INLINE __declspec(noinline)
#elif AM_VERSION_CHECK_PGI(10, 2, 0)
#  define AM_NEVER_INLINE _Pragma("noinline")
#else
#  define AM_NEVER_INLINE
#endif

/** @def AM_ATTR_NONNULL
 * @hideinitializer
 * @param args Argument indices that cannot be `NULL`. Passed as a tuple.
 * @brief Function attribute that signals arguments must not be null
 * @code size_t strlen(const char *) AM_ATTR_NONNULL((1)); @endcode
 */
#if AM_HAS_ATTRIBUTE(nonnull) || \
    AM_VERSION_CHECK_GCC(3, 3, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_ATTR_NONNULL(args) __attribute((__nonnull__ args))
#else
#  define AM_ATTR_NONNULL(args)
#endif

/** @def AM_ATTR_PRINTF
 * @hideinitializer
 * @param string_idx The index of the format string
 * @param first_to_check The first index to check
 */
#if AM_HAS_ATTRIBUTE(format) || \
    AM_VERSION_CHECK_GCC(3, 1, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_ATTR_PRINTF(string_idx, first_to_check) __attribute__((__format__(__printf__, string_idx, first_to_check)))
#else
#  define AM_ATTR_PRINTF(string_idx, first_to_check)
#endif

/** @def AM_ATTR_UNUSED
 * @hideinitializer
 */
#if AM_HAS_ATTRIBUTE(unused) || \
    AM_VERSION_CHECK_GCC(2, 7, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_ATTR_UNUSED __attribute__((unused))
#else
#  define AM_ATTR_UNUSED
#endif

/*****************************************************************************
 *
 * Usefull compatibility macros
 *
 *****************************************************************************/

/** @def AM_PRAGMA
 * @hideinitializer
 * @brief Insert a pragma as an expression
 * @note This macro takes a token stream as input, NOT a string
 */
#if AM_VERSION_CHECK_STDC(99) || \
    AM_VERSION_CHECK_CLANG(0, 0, 0) || \
    AM_VERSION_CHECK_GCC(3, 0, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    AM_VERSION_CHECK_PGI(18, 4, 0) || \
    AM_VERSION_CHECK_TINYC(0, 9, 17)
#  define AM_PRAGMA(p) _Pragma(#p)
#elif AM_VERSION_CHECK_MSVC(15, 0, 0)
#  define AM_PRAGMA(p) __pragma(p)
#else
#  define AM_PRAGMA(p)
#endif

/** @def AM_ATTR_WARN_UNUSED_RESULT
 * @hideinitializer
 * @brief Function attribute that signals the function's return value should
 * be inspected.
 * Good example of such a function is `realloc`.
 */
#if AM_HAS_ATTRIBUTE(warn_unused_result) || \
    AM_VERSION_CHECK_GCC(3, 4, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    AM_VERSION_CHECK_PGI(17, 10, 0)
#  define AM_ATTR_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#elif defined(_Check_return_)
#  define AM_ATTR_WARN_UNUSED_RESULT _Check_return_
#else
#  define AM_ATTR_WARN_UNUSED_RESULT
#endif

/** @def AM_NORETURN */
#if AM_HAS_ATTRIBUTE(noreturn) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    AM_VERSION_CHECK_GCC(3, 2, 0) || \
    AM_VERSION_CHECK_TINYC(0, 9, 27) /* TODO: check */
#  define AM_NORETURN __attribute__((__noreturn__))
#elif AM_VERSION_CHECK_MSVC(13, 10, 0)
#  define AM_NORETURN __declspec(noreturn)
#elif AM_VERSION_CHECK_STDC(11)
#  define AM_NORETURN _Noreturn
#else
#  define AM_NORETURN
#endif

/** @def AM_UNREACHABLE
 * @brief Signals the expression is unreachable. UB if control flow reaches here
 */
#if AM_HAS_BUILTIN(__builtin_unreachable) || \
    AM_VERSION_CHECK_GCC(4, 5, 0) || \
    AM_VERSION_CHECK_PGI(18, 10, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_UNREACHABLE() __builtin_unreachable()
#endif

/** @def AM_ASSUME
 * @brief Compiler can safely assume the expression is true. UB if false
 */
#if AM_VERSION_CHECK_MSVC(13, 10, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0)
#  define AM_ASSUME(expr) __assume(expr)
#elif AM_HAS_BUILTIN(__builtin_assume)
#  define AM_ASSUME(expr) __builtin_assume(expr)
#endif

/* Implement AM_ASSUME and AM_UNREACHABLE in terms of another */
#if !defined(AM_ASSUME) && defined(AM_UNREACHABLE)
#  define AM_ASSUME(expr) ((expr) ? 1 : (AM_UNREACHABLE(), 1))
#elif defined(AM_ASSUME) && !defined(AM_UNREACHABLE)
#  define AM_UNREACHABLE() AM_ASSUME(0)
#else
#  define AM_ASSUME(expr)  ((void)(expr))
#  define AM_UNREACHABLE() ((void)0)
#endif

/** @def AM_IMPORT */
/** @def AM_PRIVATE */
/** @def AM_PUBLIC */
#if defined(_WIN32) || defined(__CYGWIN__)
#  define AM_IMPORT  extern __declspec(dllimport)
#  define AM_PRIVATE extern
#  define AM_PUBLIC  extern __declspec(dllexport)
#else
#  if AM_HAS_ATTRIBUTE(visibility) || \
      AM_VERSION_CHECK_GCC(3, 3, 0) || \
      AM_VERSION_CHECK_INTEL(13, 0, 0) 
#    define AM_PRIVATE __attribute__((__visibility__("hidden")))
#    define AM_PUBLIC  __attribute__((__visibility__("default")))
#  else
#    define AM_PRIVATE
#    define AM_PUBLIC
#  endif
#  define AM_IMPORT extern
#endif

/** @def AM_STATIC_ASSERT
 * @note This macros uses __LINE__, so must be used on different lines
 */
#if AM_VERSION_CHECK_STDC(11) || \
    AM_HAS_FEATURE(c_static_assert) || \
    AM_VERSION_CHECK_GCC(6, 0, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    defined(_Static_assert)
#  define AM_STATIC_ASSERT(expr, message) _Static_assert(expr, message)
#elif AM_VERSION_CHECK_MSVC(16, 0, 0)
#  define AM_STATIC_ASSERT(expr, message) static_assert(expr, message)
#else
#  define AM_STATIC_ASSERT(expr, message) typedef char AM_CONCAT(am_static_assert_, __LINE__)[(expr) ? 1 : -1]
#endif

/** @def AM_FALLTHROUGH */
#if AM_HAS_ATTRIBUTE(fallthrough) || \
    AM_VERSION_CHECK_GCC(7, 0, 0)
#  define AM_FALLTHROUGH __attribute__((__fallthrough__))
#elif defined(__falthrough)
#  define AM_FALLTHROUGH __fallthrough
#else
#  define AM_FALLTHROUGH /* fallthrough */
#endif

/** @def AM_IS_CONSTEXPR */
#if AM_HAS_BUILTIN(__builtin_constant_p) || \
    AM_VERSION_CHECK_GCC(3, 4, 0) || \
    AM_VERSION_CHECK_INTEL(13, 0, 0) || \
    AM_VERSION_CHECK_TINYC(0, 9, 19)
#  define AM_IS_CONSTEXPR(expr) __builtin_constant_p(expr)
#elif AM_VERSION_CHECK_STDC(11) || AM_HAS_EXTENSION(c_generic_selections)
#  define AM_IS_CONSTEXPR(expr)                 \
    _Generic(  (1                               \
               ? (void *)(intptr_t)((expr) * 0) \
               : (int *)0                       \
            , int *: 1                          \
            , void *: 0))
#else
#  define AM_IS_CONSTEXPR(expr) (0)
#endif

/** @def AM_STRFUNC
 * @brief A `const char *` pointer to the name of the current function
 */
#if AM_VERSION_CHECK_STDC(99)
#  define AM_STRFUNC ((const char *)(__func__))
#elif AM_VERSION_CHECK_GCC(0, 0, 0) 
#  define AM_STRFUNC ((const char *)(__extension__ __FUNCTION__))
#elif AM_VERSION_CHECK_MSVC(13, 0, 0)
#  define AM_STRFUNC ((const char *)(__FUNCTION__))
#else
#  define AM_STRFUNC ((const char *)("???"))
#endif

/** @def AM_STRLOC
 * @brief A string literal representing the current code position
 */
#define AM_STRLOC __FILE__ ":" AM_STRINGIFY(__LINE__)

/*****************************************************************************
 *
 * Visibility
 *
 *****************************************************************************/

/** @def AM_IMPORT
 * @brief Visibility attribute for symbols imported from an external shared library
 */
/** @def AM_PUBLIC
 * @brief Visibility attribute for symbols exported from the current shared library
 */
/** @def AM_PRIVATE
 * @brief Visibility attribute for symbols internal to the current shared library
 */
#if defined(_WIN32) || defined(__CYGWIN__)
#  define AM_IMPORT  __declspec(dllimport)
#  define AM_PUBLIC  __declspec(dllexport)
#  define AM_PRIVATE
#else
#  if AM_HAS_ATTRIBUTE(visibility) || \
      AM_VERSION_CHECK_GCC(3, 3, 0) || \
      AM_VERSION_CHECK_INTEL(13, 0, 0)
#    define AM_PUBLIC  __attribute__((__visibility__("default")))
#    define AM_PRIVATE __attribute__((__visibility__("hidden")))
#  else
#    define AM_PUBLIC
#    define AM_PRIVATE
#  endif
#  define AM_IMPORT  extern
#endif

/** @def AM_DEPRECATED
 * @brief Issues a compiler warning on function use
 * @param msg The message to be displayed by the compiler when the function is used
 */
#if AM_VERSION_CHECK_MSVC(14, 0, 0)
#  define AM_DEPRECATED(msg) __declspec(deprecated(msg))
#elif AM_HAS_EXTENSION(attribute_deprecated_with_message) || \
      AM_VERSION_CHECK_GCC(4, 5, 0) || \
      AM_VERSION_CHECK_INTEL(13, 0, 0) || \
      AM_VERSION_CHECK_PGI(17, 10, 0)
#  define AM_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#elif AM_HAS_EXTENSION(deprecated) || \
      AM_VERSION_CHECK_GCC(3, 1, 0)
#  define AM_DEPRECATED(msg) __attribute__((__deprecated__))
#elif AM_VERSION_CHECK_MSVC(13, 10, 0)
#  define AM_DEPRECATED(msg) __declspec(deprecated)
#else
#  define AM_DEPRECATED(msg)
#endif

/** @def AM_API
 * @brief Visibility macro for functions within amlib
 * @note This value can be overridden in order to modify amlib visibility
 * This kind of macro should be defined by user code for similar functionality
 */
#ifndef AM_API
#  ifdef AM_COMPILING
#    define AM_API AM_PUBLIC
#  else
#    define AM_API AM_IMPORT
#  endif
#endif

#endif /* ifndef AM_COMMON_H */
