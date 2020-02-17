
#ifndef AM_LOGGING_H
#define AM_LOGGING_H 1

#include <stdint.h>
#include <stdarg.h>
#include "am/macros.h"

enum am_log_level_flags {
    /* Log flags */
    AM_LOG_FLAG_RECURSION = 1 << 0, /* Avoid recursion - internal flag */
    AM_LOG_FLAG_FATAL     = 1 << 1, /* Terminate post-message */

    /* Log levels - naming comes from Journald */
    AM_LOG_LEVEL_CRITICAL = 1 << 2, /* Severe, unrecoverable error */
    AM_LOG_LEVEL_ERROR    = 1 << 3, /* Error, not severe, recoverable */
    AM_LOG_LEVEL_WARNING  = 1 << 4, /* Indicates an error may occur if no action is taken */
    AM_LOG_LEVEL_NOTICE   = 1 << 5, /* Unusual, but not an error condition */
    AM_LOG_LEVEL_INFO     = 1 << 6, /* Normal operational messages, no action needed */
    AM_LOG_LEVEL_DEBUG    = 1 << 7  /* Information useful for debugging */
};

/* Mask to get the log level */
#define AM_LOG_LEVEL_MASK ~(AM_LOG_FLAG_RECURSION | AM_LOG_FLAG_FATAL)

#define AM_LOG_WRITER_HANDLED   1
#define AM_LOG_WRITER_UNHANDLED 0

struct am_log_field {
    const char *key;
    const void *value;
    intptr_t length; /* length of value, or -1 if null-terminated */
};

typedef int am_log_writer_fn(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields,
        void *userdata);

AM_PUBLIC
void am_log_structured(const char *log_domain, enum am_log_level_flags log_level, ...);
AM_PUBLIC
void am_log_structured_array(enum am_log_level_flags log_level, const struct am_log_field *fields, size_t num_fields);
AM_PUBLIC AM_ATTR_PRINTF(6, 7)
void am_log_structured_standard(
        const char *log_domain,
        enum am_log_level_flags log_level,
        const char *file, const char *line, const char *func,
        const char *fmt, ...);

AM_PUBLIC
void am_set_writer_func(am_log_writer_fn *fn, void *userdata);
AM_PUBLIC
am_log_writer_fn am_log_writer_default;

#define AM_DEBUG_HERE() \
    am_log_structured(NULL, AM_LOG_LEVEL_DEBUG, \
            "CODE_FILE", __FILE__, \
            "CODE_LINE", AM_STRINGIFY(__LINE__), \
            "CODE_FUNC", AM_STRFUNC, \
            "MESSAGE", "%"PRId64": %s", 0 /* monotonic_time() */, G_STRLOC)

#if AM_HAS_VARIADIC_MACROS == 1
AM_DIAGNOSTIC_PUSH
AM_DIAGNOSTIC_IGNORE_VARIADIC_MACROS
#  define am_error(...) \
    am_log_structured_standard( \
            NULL, AM_LOG_LEVEL_ERROR | AM_LOG_FLAG_FATAL, \
             __FILE__, AM_STRINGIFY(__LINE__), AM_STRFUNC, \
             __VA_ARGS__)
AM_DIAGNOSTIC_POP
#elif AM_HAS_VARIADIC_MACROS == 2
AM_DIAGNOSTIC_PUSH
AM_DIAGNOSTIC_IGNORE_VARIADIC_MACROS
#  define am_error(fmt...) \
    am_log_structured_standard( \
            NULL, AM_LOG_LEVEL_ERROR | AM_LOG_FLAG_FATAL, \
             __FILE__, AM_STRINGIFY(__LINE__), AM_STRFUNC, \
             fmt)
AM_DIAGNOSTIC_POP
#else
#  error NYI
#endif

#endif /* ifndef AM_LOGGING_H */
