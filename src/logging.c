
#define _GNU_SOURCE
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "am/threads.h"
#include "am/macros.h"
#include "am/logging.h"
#include "am/utils.h"

#define ANSI_RED        "\033[1;31m"
#define ANSI_GREEN      "\033[1;32m"
#define ANSI_YELLOW     "\033[1;33m"
#define ANSI_MAGENTA    "\033[1;35m"
#define ANSI_LIGHT_BLUE "\033[34m"
#define ANSI_RESET      "\033[0m"

static am_mutex g_messages_lock;
static am_tss g_log_structured_depth;
static am_log_writer_fn *g_log_writer_fn = am_log_writer_default;
static void *g_log_writer_userdata = NULL;

static am_once_flag g_initialized = AM_ONCE_FLAG_INITIALIZER;

/* Cleanup the logging library */
static
void cleanup(void)
{
    am_tss_delete(g_log_structured_depth);
    am_mutex_destroy(&g_messages_lock);
}

/* Setup the logging library */
static
void setup(void)
{
    int ret;
    ret = am_tss_create(&g_log_structured_depth, NULL);
    assert(ret == AM_THREAD_SUCCESS);

    ret = am_mutex_init(&g_messages_lock, AM_MUTEX_PLAIN);
    assert(ret == AM_THREAD_SUCCESS);

    atexit(cleanup);
}

/* abort, leaving a breakpoint if @p breakpoint is true */
static AM_NORETURN
void log_abort(bool breakpoint)
{
    /* TODO: breakpoint */
    (void)breakpoint;
    abort();
}

static AM_ATTR_RETURNS_NON_NULL
const char *log_level_to_string(enum am_log_level_flags log_level)
{
    if (log_level & AM_LOG_LEVEL_CRITICAL) {
        return "Critical";
    } else if (log_level & AM_LOG_LEVEL_ERROR) {
        return "Error";
    } else if (log_level & AM_LOG_LEVEL_WARNING) {
        return "Warning";
    } else if (log_level & AM_LOG_LEVEL_NOTICE) {
        return "Notice";
    } else if (log_level & AM_LOG_LEVEL_INFO) {
        return "Info";
    } else if (log_level & AM_LOG_LEVEL_DEBUG) {
        return "Debug";
    } else {
        return "";
    }
}

static AM_ATTR_RETURNS_NON_NULL
const char *log_level_to_priority(enum am_log_level_flags log_level)
{
    if (log_level & AM_LOG_LEVEL_CRITICAL) {
        return "2";
    } else if (log_level & AM_LOG_LEVEL_ERROR) {
        return "3";
    } else if (log_level & AM_LOG_LEVEL_WARNING) {
        return "4";
    } else if (log_level & AM_LOG_LEVEL_NOTICE) {
        return "5";
    } else if (log_level & AM_LOG_LEVEL_INFO) {
        return "6";
    } else if (log_level & AM_LOG_LEVEL_DEBUG) {
        return "7";
    } else {
        /* Default to AM_LOG_LEVEL_NOTICE */
        return "5";
    }
}

static AM_ATTR_RETURNS_NON_NULL
const char *log_level_to_color(enum am_log_level_flags log_level, bool color)
{
    if (!color) {
        return "";
    }
    if (log_level & AM_LOG_LEVEL_CRITICAL) {
        return ANSI_RED;
    } else if (log_level & AM_LOG_LEVEL_ERROR) {
        return ANSI_MAGENTA;
    } else if (log_level & AM_LOG_LEVEL_WARNING) {
        return ANSI_YELLOW;
    } else if (log_level & AM_LOG_LEVEL_NOTICE) {
        return ANSI_GREEN;
    } else if (log_level & AM_LOG_LEVEL_INFO) {
        return ANSI_GREEN;
    } else if (log_level & AM_LOG_LEVEL_DEBUG) {
        return ANSI_GREEN;
    } else {
        return "";
    }
}

static AM_ATTR_RETURNS_NON_NULL
const char *color_reset(bool color)
{
    if (!color) {
        return "";
    }
    return ANSI_RESET;
}

/****/

static
am_log_writer_fn writer_fallback;

AM_PUBLIC
void am_log_structured_array(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields)
{
    am_log_writer_fn *writer_func = writer_fallback;
    void *writer_userdata = NULL;
    unsigned depth;

    /* Ensure logging is setup */
    am_call_once(&g_initialized, setup);

    depth = (unsigned)(uintptr_t)am_tss_get(g_log_structured_depth);

    if (depth == 0) {
        am_mutex_lock(&g_messages_lock);
        {
            writer_func = g_log_writer_fn;
            writer_userdata = g_log_writer_userdata;
        }
        am_mutex_unlock(&g_messages_lock);
    }

    assert(writer_func != NULL);
    am_tss_set(g_log_structured_depth, (void *)(uintptr_t)(++depth));
    writer_func(log_level, fields, num_fields, writer_userdata);
    am_tss_set(g_log_structured_depth, (void *)(uintptr_t)(--depth));

    if (log_level & AM_LOG_FLAG_FATAL) {
        log_abort((log_level & AM_LOG_FLAG_RECURSION) ? false : true);
    }
}

AM_PUBLIC
void am_log_structured(const char *log_domain, enum am_log_level_flags log_level, ...)
{
    va_list args;
    char buffer[1024];
    const char *format;
    void *p;
    size_t n_fields, i;
    struct am_log_field fields[16];

    va_start(args, log_level);

    n_fields = 2;
    if (log_domain)
        n_fields++;

    for (p = va_arg(args, char *), i = n_fields;
            strcmp(p, "MESSAGE") != 0 && i < 16;
            p = va_arg(args, char *), i++) {
        fields[i].key = p;
        fields[i].value = va_arg(args, void *);
        fields[i].length = -1;
    }

    format = va_arg(args, const char *);
    snprintf(buffer, sizeof(buffer), format, args);
    buffer[1023] = '\0';

    fields[0].key = "MESSAGE";
    fields[0].value = buffer;
    fields[0].length = -1;
    fields[1].key = "PRIORITY";
    fields[1].value = log_level_to_priority(log_level);
    fields[1].length = -1;
    if (log_domain) {
        fields[2].key = "AM_DOMAIN";
        fields[2].value = log_domain;
        fields[2].length = -1;
    }

    am_log_structured_array(log_level, fields, n_fields);

    va_end(args);
}

AM_PUBLIC
void am_set_writer_func(am_log_writer_fn *fn, void *userdata)
{
    assert(fn != NULL);

    am_mutex_lock(&g_messages_lock);
    {
        g_log_writer_fn = fn;
        g_log_writer_userdata = userdata;
    }
    am_mutex_unlock(&g_messages_lock);
}

/* #define N (((sizeof(long) * 3) + 3) + 32) */

int am_log_writer_default(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields,
        void *userdata)
{
    size_t i;
    const char *message = NULL;
    const char *domain = NULL;
    const char *progname = am_progname();
    int message_len = 0;
    int domain_len = 0;
    bool color = true;
    char time_buf[128];
    struct timespec ts;
    struct tm *now_tm;

    (void)userdata;

    /* Search for "MESSAGE" and "AM_DOMAIN" fields */
    for (i = 0; (message == NULL || domain == NULL) && i < num_fields; i++) {
        const struct am_log_field *field = &fields[i];
        if (strcmp(field->key, "MESSAGE") == 0) {
            message = field->value;
            message_len = field->length;
        }
        if (strcmp(field->key, "AM_DOMAIN") == 0) {
            domain = field->value;
            domain_len = field->length;
        }
    }

    if (message != NULL && message_len < 0) {
        message_len = strlen(message);
    }
    if (domain != NULL && domain_len < 0) {
        domain_len = strlen(domain);
    }

    /* Print process info */
    fprintf(stderr, "(%s:%d): ", progname ? progname : "process", am_getpid());
    /* Print domain */
    if (domain) {
        fwrite(domain, sizeof(char), domain_len, stderr);
        /* fputs(domain, stderr); */
        fputs("-", stderr);
    }
    /* Print colored log level */
    fprintf(stderr, "%s%s%s: ",
            log_level_to_color(log_level, color),
            log_level_to_string(log_level),
            color_reset(color));
    /* Print timestamp */
    am_gettimeofday(&ts);
    now_tm = localtime(&ts.tv_sec);
    strftime(time_buf, sizeof time_buf, "%H:%M:%S", now_tm);
    fprintf(stderr, "%s%s.%03d%s: ",
            color ? ANSI_LIGHT_BLUE : "",
            time_buf,
            (int)((ts.tv_nsec / 1000) % 1000),
            color_reset(color));
    /* Print message */
    if (message) {
        fwrite(message, sizeof(char), message_len, stderr);
    } else {
        fputs("(NULL) message", stderr);
    }

    fputs("\n\n", stderr);
    fflush(stderr);
    return AM_LOG_WRITER_HANDLED;
}


/* Fallback writer if logging occurs while calling writer function (oops) */
static
int writer_fallback(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields,
        void *userdata)
{
    size_t i;
    (void)log_level;
    (void)userdata;

    for (i = 0; i < num_fields; i++) {
        const struct am_log_field *field = &fields[i];

        if (strcmp(field->key, "MESSAGE") == 0 ||
            strcmp(field->key, "MESSAGE_ID") == 0 ||
            strcmp(field->key, "PRIORITY") == 0 ||
            strcmp(field->key, "CODE_FILE") == 0 ||
            strcmp(field->key, "CODE_LINE") == 0 ||
            strcmp(field->key, "CODE_FUNC") == 0 ||
            strcmp(field->key, "ERRNO") == 0 ||
            strcmp(field->key, "SYSLOG_FACILITY") == 0 ||
            strcmp(field->key, "SYSLOG_PID") == 0 ||
            strcmp(field->key, "AM_DOMAIN") == 0)
        {
            fputs(field->key, stderr);
            fputs("=", stderr);
            if (field->length < 0) {
                fputs(field->value, stderr);
            } else {
                fwrite(field->value, sizeof(char), field->length, stderr);
            }
        }
    }

    fprintf(stderr, "_PID=%d", (int)am_getpid());
    return AM_LOG_WRITER_HANDLED;
}

