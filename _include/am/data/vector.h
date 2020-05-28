
#ifndef AM_DATA_VECTOR_H
#define AM_DATA_VECTOR_H 1

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "am/macros.h"

#define am_vector(ty) \
    struct { size_t size, capacity; ty *elems; }

#define AM_VECTOR_INITIALIZER { 0, 0, NULL }

#define am_vector_init(vec) do {       \
    (vec)->size = (vec)->capacity = 0; \
    (vec)->elems = NULL;               \
} while (/* CONSTCOND */ 0)

#define am_vector_destroy(vec) do { \
    free((vec)->elems);             \
    am_vector_init(vec);            \
} while (/* CONSTCOND */ 0)

#define am_vector_append(vec, elem) do {                                                  \
    if ((vec)->size == (vec)->capacity) {                                                 \
        (vec)->capacity = (vec)->capacity ? (vec)->capacity + ((vec)->capacity >> 1) : 8; \
        (vec)->elems = realloc((vec)->elems, (vec)->capacity * sizeof((vec)->elems[0]));  \
    }                                                                                     \
    (vec)->elems[(vec)->size++] = (elem);                                                 \
} while (/* CONSTCOND */ 0)

#define am_vector_append_(vec) do {                                                       \
    if ((vec)->size == (vec)->capacity) {                                                 \
        (vec)->capacity = (vec)->capacity ? (vec)->capacity + ((vec)->capacity >> 1) : 8; \
        (vec)->elems = realloc((vec)->elems, (vec)->capacity * sizeof((vec)->elems[0]));  \
    }                                                                                     \
    (vec)->size++;                                                                        \
} while (/* CONSTCOND */ 0)

#define am_vector_at(vec, idx) ((vec)->elems[idx])

#define am_vector_foreach(it, vec) \
    for ((it) = &(vec)->elems[0]; (it) != &(vec)->elems[(vec)->size]; (it)++)

#endif /* ifndef AM_DATA_VECTOR_H */
