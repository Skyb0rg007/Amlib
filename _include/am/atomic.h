
#ifndef AM_ATOMIC_H
#define AM_ATOMIC_H 1

#include <stdatomic.h>
#include <stdbool.h>
#include "am/macros.h"

enum am_memory_order {
    AM_MEMORY_ORDER_RELAXED = memory_order_relaxed,
    AM_MEMORY_ORDER_CONSUME = memory_order_consume,
    AM_MEMORY_ORDER_ACQUIRE = memory_order_acquire,
    AM_MEMORY_ORDER_RELEASE = memory_order_release,
    AM_MEMORY_ORDER_ACQ_REL = memory_order_acq_rel,
    AM_MEMORY_ORDER_SEQ_CST = memory_order_seq_cst
};

#define X(ty, atom_ty) \
    typedef struct { atomic_##atom_ty val; } am_atomic_##atom_ty; \
    static AM_INLINE void am_atomic_init_##atom_ty(volatile am_atomic_##atom_ty *x, ty y) { atomic_init(&x->val, y); } \
    static AM_INLINE ty am_atomic_load_##atom_ty(volatile am_atomic_##atom_ty *x) { return atomic_load(&x->val); } \
    static AM_INLINE ty am_atomic_load_##atom_ty##_explicit(volatile am_atomic_##atom_ty *x, enum am_memory_order order) { return atomic_load_explicit(&x->val, order); } \
    static AM_INLINE void am_atomic_store_##atom_ty(volatile am_atomic_##atom_ty *x, ty y) { atomic_store(&x->val, y); } \
    static AM_INLINE void am_atomic_store_##atom_ty##_explicit(volatile am_atomic_##atom_ty *x, ty y, enum am_memory_order order) { atomic_store_explicit(&x->val, y, order); } \
    static AM_INLINE ty am_atomic_fetch_add_##atom_ty(volatile am_atomic_##atom_ty *x, ty y) { return atomic_fetch_add(&x->val, y); } \
    static AM_INLINE ty am_atomic_fetch_add_##atom_ty##_explicit(volatile am_atomic_##atom_ty *x, ty y, enum am_memory_order order) { return atomic_fetch_add_explicit(&x->val, y, order); } \
    static AM_INLINE bool am_atomic_cas_##atom_ty(volatile am_atomic_##atom_ty *x, ty *y, ty z) { return atomic_compare_exchange_strong(&x->val, y, z); } \
    static AM_INLINE bool am_atomic_cas_##atom_ty##_explicit(volatile am_atomic_##atom_ty *x, ty *y, ty z, enum am_memory_order succ, enum am_memory_order fail) { return atomic_compare_exchange_strong_explicit(&x->val, y, z, succ, fail); }

X(unsigned int, uint)
X(int, int)

static AM_INLINE void *am_atomic_load_ptr(void *volatile *x)
{
    return atomic_load(x);
}
static AM_INLINE void *am_atomic_load_ptr_explicit(void *volatile *x, enum am_memory_order order)
{
    return atomic_load_explicit(x, order);
}
static AM_INLINE void *am_atomic_fetch_add_ptr(void *volatile *x, ptrdiff_t y)
{
    return atomic_fetch_add(x, y);
}
static AM_INLINE void *am_atomic_fetch_add_ptr_explicit(void *volatile *x, ptrdiff_t y, enum am_memory_order order)
{
    return atomic_fetch_add_explicit(x, y, order);
}
static AM_INLINE bool am_atomic_cas_ptr(void *volatile *x, void **expected, void *desired)
{
    return atomic_compare_exchange_strong(x, expected, desired);
}
static AM_INLINE bool am_atomic_cas_ptr_explicit(void *volatile *x, void **expected, void *desired, enum am_memory_order succ, enum am_memory_order fail)
{
    return atomic_compare_exchange_strong_explicit(x, expected, desired, succ, fail);
}

#undef X

#endif /* ifndef AM_ATOMIC_H */
