
#include "am/macros.h"
#include "am/concurrent/ring_buffer.h"

/* SPSC */

AM_ATTR_NON_NULL((1, 2)) AM_PUBLIC
void *am_ring_enqueue_reserve_spsc(struct am_ring *ring, void *buffer, unsigned entry_size)
{
    const unsigned mask = ring->size - 1;
    unsigned consumer, producer, delta;

    consumer = am_atomic_load_uint(&ring->c_head);
    producer = am_atomic_load_uint_explicit(&ring->p_tail, AM_MEMORY_ORDER_RELAXED);
    delta = producer + 1;

    if (AM_UNLIKELY((delta & mask) == (consumer & mask))) {
        return NULL;
    }

    return (char *)buffer + entry_size * (producer & mask);
}

AM_ATTR_NON_NULL((1)) AM_PUBLIC
void am_ring_enqueue_commit_spsc(struct am_ring *ring)
{
    am_atomic_fetch_add_uint(&ring->p_tail, 1);
}

AM_ATTR_NON_NULL((1, 2, 3)) AM_ATTR_WARN_UNUSED_RESULT AM_PUBLIC
bool am_ring_enqueue_spsc(struct am_ring *ring, void *buffer, const void *entry, unsigned entry_size)
{
    void *new_entry = am_ring_enqueue_reserve_spsc(ring, buffer, entry_size);
    if (new_entry == NULL) {
        return false;
    }
    memcpy(new_entry, entry, entry_size);
    am_ring_enqueue_commit_spsc(ring);
    return true;
}

AM_ATTR_NON_NULL((1, 2, 3)) AM_PUBLIC
bool am_ring_dequeue_spsc(struct am_ring *ring, const void *buffer, void *target, unsigned entry_size)
{
    const unsigned mask = ring->size - 1;
    unsigned consumer, producer;

    consumer = am_atomic_load_uint_explicit(&ring->c_head, AM_MEMORY_ORDER_RELAXED);
    producer = am_atomic_load_uint(&ring->p_tail);

    if (AM_UNLIKELY(consumer == producer)) {
        return false;
    }

    buffer = (const char *)buffer + entry_size * (consumer & mask);
    memcpy(target, buffer, entry_size);

    am_atomic_store_uint(&ring->c_head, consumer + 1);
    return true;
}

/*****************************************************************************/

/* MPMC */

AM_ATTR_NON_NULL((1, 2, 3)) AM_ATTR_WARN_UNUSED_RESULT AM_PUBLIC
bool am_ring_enqueue_mpmc(struct am_ring *ring, void *buffer, const void *entry, unsigned entry_size)
{
    unsigned ticket;
    void *new_entry = am_ring_enqueue_reserve_mpmc(ring, buffer, entry_size, &ticket);
    if (new_entry == NULL) {
        return false;
    }
    memcpy(new_entry, entry, entry_size);
    am_ring_enqueue_commit_mpmc(ring, ticket);
    return true;
}

AM_ATTR_NON_NULL((1, 2)) AM_PUBLIC
void *am_ring_enqueue_reserve_mpmc(struct am_ring *ring, void *buffer, unsigned entry_size, unsigned *ticket)
{
    const unsigned mask = ring->size - 1;
    unsigned producer, consumer, delta;

    producer = am_atomic_load_uint(&ring->p_head);

    for (;;) {
        consumer = am_atomic_load_uint(&ring->c_head);
        delta = producer + 1;

        if (AM_LIKELY((producer - consumer) < mask)) {
            /* Swing the p_head from 'producer' to 'producer + 1' */
            if (am_atomic_cas_uint(&ring->p_head, &producer, delta)) {
                break;
            }
        } else {
            unsigned new_producer;
            new_producer = am_atomic_load_uint(&ring->p_head);
            if (producer == new_producer) {
                return false;
            }
            producer = new_producer;
        }
    }
    *ticket = producer;
    return (char *)buffer + entry_size * (producer & mask);
}

AM_ATTR_NON_NULL((1)) AM_PUBLIC
void am_ring_enqueue_commit_mpmc(struct am_ring *ring, unsigned ticket)
{
    while (am_atomic_load_uint(&ring->p_tail) != ticket)
        ;
    am_atomic_store_uint(&ring->p_tail, ticket + 1);
}

AM_ATTR_NON_NULL((1, 2, 3)) AM_PUBLIC
bool am_ring_dequeue_mpmc(struct am_ring *ring, const void *buffer, void *data, unsigned entry_size)
{
    const unsigned mask = ring->size - 1;
    unsigned consumer, producer;

    consumer = am_atomic_load_uint(&ring->c_head);

    do {
        const void *target;

        producer = am_atomic_load_uint(&ring->p_tail);
        if (AM_UNLIKELY(consumer == producer)) {
            return false;
        }

        target = (const char *)buffer + entry_size * (consumer & mask);
        memcpy(data, target, entry_size);

    } while (!am_atomic_cas_uint(&ring->c_head, &consumer, consumer + 1));
    return true;
}

