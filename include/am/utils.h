
#ifndef AM_UTILS_H
#define AM_UTILS_H 1

#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "am/macros.h"

#if defined(AM_HAS_GLIBC_PROGNAME)
static AM_INLINE AM_ATTR_RETURNS_NON_NULL
const char *am_progname(void)
{
    extern const char *__progname;
    return __progname;
}
#elif 1
#  error NYI
#endif

/* static AM_INLINE */
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

#endif /* ifndef AM_UTILS_H */
