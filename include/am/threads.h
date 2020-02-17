
#ifndef AM_THREADS_H
#define AM_THREADS_H 1

#define _GNU_SOURCE

#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sched.h> /* sched_yield */
#include "am/macros.h"

typedef pthread_t          am_thread;
typedef pthread_mutex_t    am_mutex;
typedef pthread_cond_t     am_cond;
typedef pthread_key_t      am_tss;
typedef pthread_spinlock_t am_spinlock;
typedef pthread_once_t     am_once_flag;

#define AM_ONCE_FLAG_INITIALIZER PTHREAD_ONCE_INIT

typedef int (*am_thread_fn)(void *);

enum am_mutex_type {
    AM_MUTEX_PLAIN           = 0, /**< Simple, non-recursive mutex           */
    AM_MUTEX_TIMED           = 1, /**< Non-recursive mutex, supports timeout */
    AM_MUTEX_RECURSIVE       = 2  /**< Recursive mutex                       */
};

enum am_thread_error {
    AM_THREAD_SUCCESS  = 0,
    AM_THREAD_TIMEDOUT = 1,
    AM_THREAD_BUSY     = 2,
    AM_THREAD_ERROR    = 3,
    AM_THREAD_NOMEM    = 4
};

/* Threads */

/** @brief Create a thread
 * @param t Returns the thread identifier
 * @param fn The function to run on the new thread
 * @param userdata Data to pass the function
 */
static AM_INLINE enum am_thread_error
am_thread_create(am_thread *t, am_thread_fn fn, void *userdata)
{
AM_DIAGNOSTIC_PUSH
AM_DIAGNOSTIC_IGNORE_CAST_FUNC_TYPE
    int res = pthread_create(t, 0, (void *(*)(void *))fn, userdata);
AM_DIAGNOSTIC_POP
    if (res == 0) {
        return AM_THREAD_SUCCESS;
    } else if (res == ENOMEM) {
        return AM_THREAD_NOMEM;
    } else {
        return AM_THREAD_ERROR;
    }
}

/** @brief Exit the current thread
 * @param res The value to exit the thread with
 */
static AM_INLINE void
am_thread_exit(int res)
{
    pthread_exit((void *)(intptr_t)res);
}

/** @brief Wait for the specified thread to terminate
 * @param t The thread id to check
 * @param res Pointer for the thread's return value
 */
static AM_INLINE enum am_thread_error
am_thread_join(am_thread t, int *res)
{
    void *retval;

    if (pthread_join(t, &retval) != 0) {
        return AM_THREAD_ERROR;
    }
    if (res != NULL) {
        *res = (intptr_t)retval;
    }
    return AM_THREAD_SUCCESS;
}

/** @brief Mark a thread as detached, ie. will no be joined
 * @param t The thread id to mark
 */
static AM_INLINE enum am_thread_error
am_thread_detach(am_thread t)
{
    return pthread_detach(t) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/** @brief Access the current thread id */
static AM_INLINE am_thread
am_thread_current(void)
{
    return pthread_self();
}

/** @brief Compare two thread ids for equality */
static AM_INLINE bool
am_thread_equal(am_thread a, am_thread b)
{
    return pthread_equal(a, b);
}

/** @brief Suspend the calling thread until the time specified has passed
 * @param ts_in The amount of time to sleep
 * @param ts_out The time not slept, if inturrupted
 * @return -1 if interrupted, -2 for other errors, or 0 on success
 */
static AM_INLINE int
am_thread_sleep(const struct timespec *ts_in, struct timespec *ts_out)
{
    errno = 0;
    if (nanosleep(ts_in, ts_out) < 0) {
        if (errno == EINTR) {
            return -1;
        } else {
            return -2;
        }
    }
    return 0;
}

/** @brief Cause the current thread to reliquish to the CPU */
static AM_INLINE void
am_thread_yield(void)
{
    sched_yield();
}

/* Mutex */

/** @brief Initialize a mutex */
static AM_INLINE enum am_thread_error
am_mutex_init(am_mutex *mtx, enum am_mutex_type type)
{
    int res;
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    if (type == AM_MUTEX_RECURSIVE) {
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    } else if (type == AM_MUTEX_PLAIN) {
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    } else if (type == AM_MUTEX_TIMED) {
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_TIMED_NP);
    }
    res = pthread_mutex_init(mtx, &attr);
    pthread_mutexattr_destroy(&attr);
    return res == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/** @brief Destroy a mutex */
static AM_INLINE void
am_mutex_destroy(am_mutex *mtx)
{
    pthread_mutex_destroy(mtx);
}

/** @brief Acquire a mutex */
static AM_INLINE enum am_thread_error
am_mutex_lock(am_mutex *mtx)
{
    int res = pthread_mutex_lock(mtx);
    if (res == EDEADLK) {
        return AM_THREAD_BUSY;
    } else if (res == 0) {
        return AM_THREAD_SUCCESS;
    } else {
        return AM_THREAD_ERROR;
    }
}

/** @brief Try to acquire a mutex */
static AM_INLINE enum am_thread_error
am_mutex_trylock(am_mutex *mtx)
{
    int res = pthread_mutex_trylock(mtx);
    if (res == EBUSY) {
        return AM_THREAD_BUSY;
    } else if (res == 0) {
        return AM_THREAD_SUCCESS;
    } else {
        return AM_THREAD_ERROR;
    }
}

/** @brief Try to acquire a mutex within a period of time
 * @param mtx A timed mutex
 * @param ts A timespec representing the amount of time to wait for the lock
 */
static AM_INLINE enum am_thread_error
am_mutex_timedlock(am_mutex *mtx, const struct timespec *ts)
{
    int res;

    res = pthread_mutex_timedlock(mtx, ts);
    if (res == ETIMEDOUT) {
        return AM_THREAD_TIMEDOUT;
    } else if (res == 0) {
        return AM_THREAD_SUCCESS;
    } else {
        return AM_THREAD_ERROR;
    }
}

/** @brief Release a mutex */
static AM_INLINE enum am_thread_error
am_mutex_unlock(am_mutex *mtx)
{
    return pthread_mutex_unlock(mtx) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/* Condition variables */

/** @brief Initialize a conditional variable */
static AM_INLINE enum am_thread_error
am_cond_init(am_cond *cond)
{
    return pthread_cond_init(cond, 0) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/** @brief Destroy a conditional variable */
static AM_INLINE void
am_cond_destroy(am_cond *cond)
{
    pthread_cond_destroy(cond);
}

/** @brief Awaken one thread waiting on the conditional variable */
static AM_INLINE enum am_thread_error
am_cond_signal(am_cond *cond)
{
    return pthread_cond_signal(cond) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/** @brief Awaken all threads waiting on the conditional variable */
static AM_INLINE enum am_thread_error
am_cond_broadcast(am_cond *cond)
{
    return pthread_cond_broadcast(cond) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/** @brief Block on the conditional variable 
 * @param cond The condition variable to block on
 * @param mtx A locked mutex that is atomically unlocked during the function call
 */
static AM_INLINE enum am_thread_error
am_cond_wait(am_cond *cond, am_mutex *mtx)
{
    return pthread_cond_wait(cond, mtx) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/** @brief Like am_cond_wait, but with a timeout */
static AM_INLINE enum am_thread_error
am_cond_timedwait(am_cond *cond, am_mutex *mtx, const struct timespec *ts)
{
    int res;

    res = pthread_cond_timedwait(cond, mtx, ts);
    if (res == ETIMEDOUT) {
        return AM_THREAD_TIMEDOUT;
    } else if (res == 0) {
        return AM_THREAD_SUCCESS;
    } else {
        return AM_THREAD_ERROR;
    }
}

/* Thread-specific data */

/** @brief Create thread-specific storage */
static AM_INLINE enum am_thread_error
am_tss_create(am_tss *key, void (*dtor)(void *))
{
    return pthread_key_create(key, dtor) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

/** @brief Delete thread-specific storage */
static AM_INLINE void
am_tss_delete(am_tss key)
{
    pthread_key_delete(key);
}

static AM_INLINE enum am_thread_error
am_tss_set(am_tss key, void *val)
{
    return pthread_setspecific(key, val) == 0 ? AM_THREAD_SUCCESS : AM_THREAD_ERROR;
}

static AM_INLINE void *
am_tss_get(am_tss key)
{
    return pthread_getspecific(key);
}

/* Once */

static AM_INLINE void
am_call_once(am_once_flag *flag, void (*fn)(void))
{
    pthread_once(flag, fn);
}

/* Spinlocks */

static AM_INLINE enum am_thread_error
am_spinlock_init(am_spinlock *lock)
{
    /* no sharing w/ other processes */
    int ret;
    ret = pthread_spin_init(lock, PTHREAD_PROCESS_PRIVATE);
    if (ret == 0) {
        return AM_THREAD_SUCCESS;
    } else if (ret == ENOMEM) {
        return AM_THREAD_NOMEM;
    } else {
        return AM_THREAD_ERROR;
    }
}

static AM_INLINE void
am_spinlock_destroy(am_spinlock *lock)
{
    pthread_spin_destroy(lock);
}

static AM_INLINE enum am_thread_error
am_spinlock_lock(am_spinlock *lock)
{
    int ret;
    ret = pthread_spin_lock(lock);
    if (ret == EDEADLK) {
        return AM_THREAD_BUSY;
    } else if (ret == 0) {
        return AM_THREAD_SUCCESS;
    } else {
        return AM_THREAD_ERROR;
    }
}

static AM_INLINE enum am_thread_error
am_spinlock_trylock(am_spinlock *lock)
{
    int ret;
    ret = pthread_spin_trylock(lock);
    if (ret == EBUSY) {
        return AM_THREAD_BUSY;
    } else if (ret == 0) {
        return AM_THREAD_SUCCESS;
    } else {
        return AM_THREAD_SUCCESS;
    }
}

static AM_INLINE enum am_thread_error
am_spinlock_unlock(am_spinlock *lock)
{
    int ret;
    ret = pthread_spin_unlock(lock);
    if (ret == 0) {
        return AM_THREAD_SUCCESS;
    } else {
        return AM_THREAD_ERROR;
    }
}

#endif /* ifndef AM_THREADS_H */
