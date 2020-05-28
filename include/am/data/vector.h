/** @file am/data/vector.h
 * @brief Auto-expanding vector
 */
#ifndef AM_DATA_VECTOR_H
#define AM_DATA_VECTOR_H 1

#include "am/common.h"
#include "am/logging.h"
#include "am/alloc.h"

struct am_vector_info {
    struct am_alloc alloc;
    struct am_relocate relocate;
};

static AM_ATTR_UNUSED const struct am_vector_info am_vector_info_pod = {
    am_alloc_standard,
    { NULL, NULL }
};

/** @brief Vector containing elements of a given type @hideinitializer */
#define am_vector(type) \
    struct { type *_begin, *_end, *_end_cap; }

/** @brief Initialize a vector */
#define am_vector_init(v) \
    ((void)((v)->_begin = (v)->_end = (v)->_end_cap = NULL))
/** @brief Free a vector */
#define am_vector_free(v, info) \
    am_free(&(info)->alloc, (v)->_begin, am_vector_capacity(v) * sizeof(*(v)->_begin))
/** @brief Vector's capacity */
#define am_vector_capacity(v) ((size_t)((v)->_end_cap - (v)->_begin))
/** @brief Vector's size */
#define am_vector_size(v) ((size_t)((v)->_end - (v)->_begin))
#define am_vector_at(v, i) ((v)->_begin[i])

#define am_vector_reserve(v, n, info)                                           \
    ((n) > am_vector_capacity(v)                                                     \
     ? am__vector_setcap((struct am__vector *)v, sizeof(*(v)->_begin), n, info) \
     : true)

#define am_vector_emplace_back(v, info)                                                                             \
    ((am_vector_size(v) == am_vector_capacity(v)                                                                    \
      ? am__vector_setcap((struct am__vector *)v, sizeof(*(v)->_begin), AM_MAX(2, am_vector_capacity(v) * 2), info) \
      : true)                                                                                                       \
        ? (v)->_end++                                                                                               \
        : NULL)

#define am_vector_shrink_to_fit(v, info)                                                         \
    (am_vector_size(v) == am_vector_capacity(v)                                                  \
     ? true                                                                                      \
     : am__vector_setcap((struct am__vector *)v, sizeof(*(v)->_begin), am_vector_size(v), info))

#define am_vector_free_pod(v)          am_vector_free(v, &am_vector_info_pod)
#define am_vector_reserve_pod(v, n)    am_vector_reserve(v, n, &am_vector_info_pod)
#define am_vector_emplace_back_pod(v)  am_vector_emplace_back(v, &am_vector_info_pod)
#define am_vector_shrink_to_fit_pod(v) am_vector_shrink_to_fit(v, &am_vector_info_pod)

/** @section Iteration Iterating over vectors */
#define am_vector_begin(v)  ((v)->_begin)
#define am_vector_end(v)    ((v)->_end)
#define am_vector_rbegin(v) ((v)->_end - 1)
#define am_vector_rend(v)   ((v)->_begin - 1)
#define am_vector_next(it)  ((it) + 1)
#define am_vector_prev(it)  ((it) - 1)
#define am_vector_foreach(it, v) \
    for ((it) = am_vector_begin(v); (it) != am_vector_end(v); (it) = am_vector_next(it))
#define am_vector_foreach_rev(it, v) \
    for ((it) = am_vector_rbegin(v); (it) != am_vector_rend(v); (it) = am_vector_prev(it))

/****************************************************************************/

struct am__vector {
    void *_begin, *_end, *_end_cap;
};

static AM_INLINE bool am__vector_setcap(struct am__vector *_v, size_t _data_size, size_t _newcap, const struct am_vector_info *_info)
{
    void *_newbegin, *_oldbegin;
    size_t _oldcap_total, _newcap_total, _size_total;

    /* Save old data */
    _oldbegin = _v->_begin;
    _oldcap_total = (char *)_v->_end_cap - (char *)_v->_begin;
    _newcap_total = _newcap * _data_size;
    _size_total = (char *)_v->_end - (char *)_v->_begin;

    /* Realloc */
    _newbegin = am_realloc((struct am_alloc *)&_info->alloc, _v->_begin, _oldcap_total, _newcap_total);
    if (!_newbegin) {
        /* TODO: make "CODE_LINE" etc. match the macro, not this function. Do after fully documenting. */
        am_log(AM_LOG_LEVEL_ERROR, AM_LOG_STD_FLDS,
                "AMLIB_DOMAIN", "amlib",
                "MESSAGE", "am_vector_reserve(%p, %zu, %p): Unable to allocate memory", (void *)_v, _newcap, (void *)_info);
        return false;
    }
    _v->_begin = _newbegin;
    _v->_end = (char *)_newbegin + _size_total;
    _v->_end_cap = (char *)_newbegin + _newcap_total;

    /* Relocate */
    if (_info->relocate.fn) {
        size_t _i;
        for (_i = 0; _i < _size_total; _i += _data_size) {
            am_relocate(&_info->relocate, (char *)_v->_begin + _i, (char *)_oldbegin + _i);
        }
    }
    return true;
}

#endif /* ifndef AM_DATA_VECTOR_H */
