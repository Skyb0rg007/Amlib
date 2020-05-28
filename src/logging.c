
#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "am/common.h"
#include "am/threads.h"
#include "am/logging.h"

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

static void fini(void)
{
    am_tss_delete(g_log_structured_depth);
    am_mutex_destroy(&g_messages_lock);
}
static void init(void)
{
    int ret;
    ret = am_tss_create(&g_log_structured_depth, NULL);
    if (ret != AM_THREAD_SUCCESS) {
        abort();
    }

    ret = am_mutex_init(&g_messages_lock, AM_MUTEX_PLAIN);
    if (ret != AM_THREAD_SUCCESS) {
        abort();
    }

    atexit(fini);
}
static void ensure_initialized(void)
{
    static am_once_flag initialized = AM_ONCE_FLAG_INITIALIZER;

    am_call_once(&initialized, init);
}

static const char *log_level_to_string(enum am_log_level_flags log_level)
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

static const char *log_level_to_priority(enum am_log_level_flags log_level)
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

static const char *log_level_to_color(enum am_log_level_flags log_level, bool color)
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

static const char *color_reset(bool color)
{
    if (!color) {
        return "";
    }
    return ANSI_RESET;
}

static am_log_writer_fn writer_fallback;

/****/

AM_API
void am_log_set_writer(am_log_writer_fn *fn, void *ud)
{
    ensure_initialized();
    if (!fn) {
        fn = am_log_writer_default;
        ud = NULL;
    }

    am_mutex_lock(&g_messages_lock);
    {
        g_log_writer_fn = fn;
        g_log_writer_userdata = ud;
    }
    am_mutex_unlock(&g_messages_lock);
}

AM_API
int am_log_writer_default(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields,
        void *ud)
{
    size_t i;
    const char *message = NULL;
    const char *domain = NULL;
    int message_len = 0;
    int domain_len = 0;
    bool color = true;
    char time_buf[128];
    time_t now_time;
    struct tm *now_tm;

    (void)ud;

    for (i = 0; (message == NULL || domain == NULL) && i < num_fields; i++) {
        const struct am_log_field *field = &fields[i];
        if (strcmp(field->key, "MESSAGE") == 0) {
            message = field->value;
            message_len = field->length;
        }
        if (strcmp(field->key, "AMLIB_DOMAIN") == 0) {
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

    if (domain) {
        fwrite(domain, sizeof(char), domain_len, stderr);
        fputs("-", stderr);
    }
    fprintf(stderr, "%s%s%s: ",
            log_level_to_color(log_level, color),
            log_level_to_string(log_level),
            color_reset(color));
    time(&now_time);
    now_tm = localtime(&now_time);
    strftime(time_buf, sizeof time_buf, "%H:%M:%S", now_tm);
    fprintf(stderr, "%s%s%s\n",
            color ? ANSI_LIGHT_BLUE : "",
            time_buf,
            color_reset(color));
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
static int writer_fallback(
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

        if (strcmp(field->key, "MESSAGE")         == 0 ||
            strcmp(field->key, "MESSAGE_ID")      == 0 ||
            strcmp(field->key, "PRIORITY")        == 0 ||
            strcmp(field->key, "CODE_FILE")       == 0 ||
            strcmp(field->key, "CODE_LINE")       == 0 ||
            strcmp(field->key, "CODE_FUNC")       == 0 ||
            strcmp(field->key, "ERRNO")           == 0 ||
            strcmp(field->key, "SYSLOG_FACILITY") == 0 ||
            strcmp(field->key, "SYSLOG_PID")      == 0 ||
            strcmp(field->key, "AMLIB_DOMAIN")    == 0)
        {
            fputs(field->key, stderr);
            fputs("=", stderr);
            if (field->length < 0) {
                fputs(field->value, stderr);
            } else {
                fwrite(field->value, sizeof(char), field->length, stderr);
            }
            fputs("\n", stderr);
        }
    }

    /* fprintf(stderr, "_PID=%d", (int)am_getpid()); */
    return AM_LOG_WRITER_HANDLED;
}

AM_API void am_log(enum am_log_level_flags log_level, ...)
{
    va_list args;
    va_start(args, log_level);
    am_logv(log_level, args);
    va_end(args);
}

AM_API void am_logv(enum am_log_level_flags log_level, va_list args)
{
    char msg_buffer[1024];
    size_t n_fields;
    struct am_log_field fields[16];
    const char *p;
    const char *format;

    ensure_initialized();

    n_fields = 2;
    for (p = va_arg(args, const char *);
            strcmp(p, "MESSAGE") != 0 && n_fields < 16;
            p = va_arg(args, const char *), n_fields++) {
        fields[n_fields].key = p;
        fields[n_fields].value = va_arg(args, const void *);
        fields[n_fields].length = -1;
    }

    format = va_arg(args, const char *);
    vsnprintf(msg_buffer, sizeof msg_buffer, format, args);
    msg_buffer[sizeof msg_buffer - 1] = '\0';

    fields[0].key = "MESSAGE";
    fields[0].value = msg_buffer;
    fields[0].length = -1;

    fields[1].key = "PRIORITY";
    fields[1].value = log_level_to_priority(log_level);
    fields[1].length = -1;

    am_log_array(log_level, fields, n_fields);

    va_end(args);
}

AM_API void am_log_array(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields)
{
    am_log_writer_fn *writer_func = writer_fallback;
    void *writer_ud = NULL;
    unsigned depth;

    ensure_initialized();

    if (!am_contract("amlib", AM_LOG_LEVEL_ERROR, fields != NULL))
        return;
    if (!am_contract("amlib", AM_LOG_LEVEL_ERROR, num_fields > 0))
        return;

    depth = (unsigned)(uintptr_t)am_tss_get(g_log_structured_depth);

    if (depth == 0) {
        am_mutex_lock(&g_messages_lock);
        {
            writer_func = g_log_writer_fn;
            writer_ud = g_log_writer_userdata;
        }
        am_mutex_unlock(&g_messages_lock);
    }

    am_tss_set(g_log_structured_depth, (void *)(uintptr_t)(depth + 1));
    writer_func(log_level, fields, num_fields, writer_ud);
    am_tss_set(g_log_structured_depth, (void *)(uintptr_t)depth);

    if (log_level & AM_LOG_FLAG_FATAL) {
        abort();
    }
}

