/** @file am/logging.h
 * @brief Logging interface
 *
 * The recommended fields:
 * AMLIB_DOMAIN - the domain of the logging. "amlib" for amlib functions.
 * MESSAGE - The human-readable message string.
 * MESSAGE_ID - 128-bit identifier. Created using `systemd-id128 new`.
 * PRIORITY - Syslog priority value. Set automatically by logging functions based on log level.
 * CODE_FILE, CODE_LINE, CODE_FUNC - Source code info.
 * ERRNO - value of `errno`.
 */

#ifndef AM_LOGGING_H
#define AM_LOGGING_H 1

#include "am/common.h"

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
#define AM_LOG_LEVEL_MASK (~(AM_LOG_FLAG_RECURSION | AM_LOG_FLAG_FATAL))

#define AM_LOG_WRITER_HANDLED   1
#define AM_LOG_WRITER_UNHANDLED 0

/** @brief Data field to be logged */
struct am_log_field {
    const char *key;
    const void *value;
    intptr_t length; /* length of value, or -1 if null-terminated */
};

/** @brief Function type for handling log events */
typedef int am_log_writer_fn(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields,
        void *userdata);

/** @brief Emit a log event
 * @param log_level The severity level
 * @param ... Logging field names and values.
 * @note If the "MESSAGE" field is provided, it must be the final field. The
 * following arguments are passed to `sprintf` with a 2048-byte buffer.
 */
AM_API void am_log(
        enum am_log_level_flags log_level,
        ...);
/** @brief Emit a log event
 * @param log_level The severity level
 * @param args Logging field names and values.
 * @note If the "MESSAGE" field is provided, it must be the final field. The
 * following arguments are passed to `sprintf` with a 1024-byte buffer.
 */
AM_API void am_logv(
        enum am_log_level_flags log_level,
        va_list args);
/** @brief Emit a log event
 * @param log_level The severity level
 * @param fields Logging fields
 * @param num_fields The number of fields
 */
AM_API void am_log_array(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields);

/** @brief Set the log writer function
 * @param fn The writer function. If `NULL`, set to the default.
 * @param userdata The writer function userdata
 */
AM_API void am_log_set_writer(am_log_writer_fn *fn, void *userdata);
/** @brief The default writer function, writes to `stderr`  */
AM_API am_log_writer_fn am_log_writer_default;

#define AM_LOG_STD_FLDS                  \
    "CODE_FILE", __FILE__,               \
    "CODE_LINE", AM_STRINGIFY(__LINE__), \
    "CODE_FUNC", AM_STRFUNC

#define AM_DEBUG_HERE()                               \
    am_log(NULL, AM_LOG_LEVEL_DEBUG,                  \
            AM_LOG_STD_FLDS                           \
            "MESSAGE", "AM_DEBUG_HERE(): " AM_STRLOC)

#define am_contract(domain, level, expr) \
    ((expr) \
     ? true \
     : (am_log(level, AM_LOG_STD_FLDS, "AMLIB_DOMAIN", domain, "MESSAGE", "%s: contract violation: \"%s\"", AM_STRFUNC, #expr), false))

#endif /* ifndef AM_LOGGING_H */
