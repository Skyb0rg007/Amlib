/** @file am/macros.h
 * @brief Generally useful macro definitions
 */

#ifndef AM_MACROS_H
#define AM_MACROS_H 1

/** @def AM_C95
 * @brief Whether C95 is supported
 */
/** @def AM_C99
 * @brief Whether C99 is supported
 */
/** @def AM_C11
 * @brief Whether C11 is supported
 */
/** @def AM_C17
 * @brief Whether C17 is supported
 */
#if !defined(__STDC_VERSION__)
#  define AM_C95 0
#  define AM_C99 0
#  define AM_C11 0
#  define AM_C17 0
#elif __STDC_VERSION__ < 199901L
#  define AM_C95 1
#  define AM_C99 0
#  define AM_C11 0
#  define AM_C17 0
#elif __STDC_VERSION__ < 201112L
#  define AM_C95 1
#  define AM_C99 1
#  define AM_C11 0
#  define AM_C17 0
#elif __STDC_VERSION__ < 201710L
#  define AM_C95 1
#  define AM_C99 1
#  define AM_C11 1
#  define AM_C17 0
#elif __STDC_VERSION__ >= 201710L
#  define AM_C95 1
#  define AM_C99 1
#  define AM_C11 1
#  define AM_C17 1
#endif
/** @brief Convert a symbol to a string, expanding macros */
#define AM_STRINGIFY(x)  AM__STRINGIFY(x)
#define AM__STRINGIFY(x) #x
/** @brief Concatenate two symbols, expanding macros */
#define AM_CONCAT(a, b)  AM__CONCAT(a, b)
#define AM__CONCAT(a, b) a##b

/* Math */

/** @brief Numerical maximum */
#define AM_MAX(a, b)                   ((a) > (b) ? (a) : (b))
/** @brief Numerical minimum */
#define AM_MIN(a, b)                   ((a) < (b) ? (a) : (b))
/** @brief Absolute value */
#define AM_ABS(a)                      ((a) < 0 ? -(a) : (a))
/** @brief Clamp a number between two values */
#define AM_CLAMP(x, low, high)         (AM_MAX(low, (AM_MIN(x, high))))
/** @brief Determine if two numbers are within @p epsilon of another
 * This is meant for testing floating-point equality
 */
#define AM_APPROX_EQUAL(a, b, epsilon) (AM_ABS((a) - (b)) < (epsilon))

/* Compiler-specific */

#if defined(_MSVC_VER)
/* MSVC */
#  include "am/macros/msvc.h"
#elif defined(__clang__)
/* Clang */
#  include "am/macros/clang.h"
#elif defined(__GNUC__)
/* GCC */
#  include "am/macros/gnu.h"
#else
/* Standard C */
#  include "am/macros/ansi.h"
#endif

#endif /* ifndef AM_MACROS_H */
