
#ifndef AM_UTILS_H
#define AM_UTILS_H 1

#define _GNU_SOURCE
#define __USE_ISOC11
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "am/macros.h"
#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif

#if AM_HAS_BUILTIN_POPCOUNT
static AM_INLINE
int am_popcount32(uint32_t n) {
    AM_STATIC_ASSERT(sizeof(unsigned int) == sizeof(uint32_t), "");
    return __builtin_popcount(n);
}

static AM_INLINE
int am_popcount64(uint64_t n) {
    AM_STATIC_ASSERT(sizeof(unsigned long) == sizeof(uint64_t), "");
    return __builtin_popcountl(n);
}
#else
#  error "NYI"
#endif

#if AM_HAS_GLIBC_PROGNAME
static AM_INLINE AM_ATTR_RETURNS_NON_NULL
const char *am_progname(void)
{
    extern const char *__progname;
    return __progname;
}
#elif 1
#  error NYI
#endif

#include <unistd.h>
#include <sys/types.h>
static AM_INLINE
int am_getpid(void)
{
    return getpid();
}

#include <sys/time.h>
/** @brief Load timespec with wall clock time */
static AM_INLINE AM_ATTR_NON_NULL((1))
bool am_gettimeofday(struct timespec *ts)
{
    struct timeval tv;
    int ret;
    ret = gettimeofday(&tv, NULL);
    ts->tv_sec = tv.tv_sec;
    ts->tv_nsec = 1000 * tv.tv_usec;
    return ret == 0;
}

/** @brief Load timespec with time since epoch */
static AM_INLINE AM_ATTR_NON_NULL((1))
bool am_gettimespec(struct timespec *ts)
{
    return !timespec_get(ts, TIME_UTC);
}

/** @brief Display a simple message box */
static AM_INLINE AM_ATTR_NON_NULL((1, 2))
bool am_show_messagebox(const char *title, const char *message)
{
    (void)title;
    (void)message;
#ifdef __EMSCRIPTEN__
    EM_ASM_({
        alert(UTF8ToString($0) + "\n\n" + UTF8ToString($1));
    }, title, message);
    return true;
#else
    return false;
#endif
}

static AM_INLINE AM_ATTR_NON_NULL((1))
char *am_strdup(const char *s)
{
#ifdef AM_HAS_STRDUP
    return strdup(s);
#else
    size_t len;
    char *ret;

    len = strlen(s);
    ret = calloc(len + 1, sizeof(char));
    if (!ret) {
        return NULL;
    }
    memcpy(ret, s, len);
    return ret;
#endif
}

#endif /* ifndef AM_UTILS_H */
