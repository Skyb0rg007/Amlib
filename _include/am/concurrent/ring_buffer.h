
#ifndef AM_CONCURRENT_RING_BUFFER_H
#define AM_CONCURRENT_RING_BUFFER_H 1

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "am/macros.h"
#include "am/atomic.h"

#define AM_CACHELINE 64

struct am_ring {
    am_atomic_uint c_head;
    /* char _pad[AM_CACHELINE - sizeof(am_atomic_uint)]; */
    am_atomic_uint p_tail;
    am_atomic_uint p_head;
    /* char pad[AM_CACHELINE - 2 * sizeof(am_atomic_uint)]; */
    unsigned size;
};
AM_STATIC_ASSERT(sizeof(struct am_ring) == 16, "");

/** @brief Determine the number of elements in the ring
 * @param ring The ring handle
 * @return The number of elements currently in the ring
 */
static AM_INLINE
unsigned am_ring_size(struct am_ring *ring)
{
    unsigned c, p;
    c = am_atomic_load_uint(&ring->c_head);
    p = am_atomic_load_uint(&ring->p_tail);
    return (p - c) & (ring->size - 1);
}

/** @brief Determine the maximum capacity of the ring
 * @param ring The ring handle
 * @return The number of possible elements in the ring
 */
static AM_INLINE
unsigned am_ring_capacity(const struct am_ring *ring)
{
    return ring->size;
}

/** @brief Initialize a ring
 * @param ring The ring buffer handle
 * @param size The number of elements in the buffer, must be power of 2
 */
static AM_INLINE
void am_ring_init(struct am_ring *ring, unsigned size)
{
    ring->size = size;
    am_atomic_init_uint(&ring->p_tail, 0);
    am_atomic_init_uint(&ring->p_head, 0);
    am_atomic_init_uint(&ring->c_head, 0);
}

/** @brief Check if the ring is in a consistent state
 * @note Not threadsafe
 */
static AM_INLINE
bool am_ring_valid(struct am_ring *ring)
{
    unsigned size = ring->size;
    unsigned c_head = am_atomic_load_uint(&ring->c_head);
    unsigned p_head = am_atomic_load_uint(&ring->p_head);

    /* size must be a power of 2 */
    if (size & (size - 1)) {
        return false;
    }

    /* Consumer must be smaller than producer */
    if (c_head > p_head) {
        return false;
    }

    /* Producer may only be up to 'size' slots ahead of consumer */
    if (p_head - c_head >= size) {
        return false;
    }

    return true;
}

/** @brief Enqueue an entry
 * @param ring Handle to the ring buffer header
 * @param buffer The buffer itself
 * @param entry The entry to enqueue
 * @param entry_size The size, in bytes, of the entry
 * @return false if the ring buffer is full, or true on success
 */
AM_ATTR_NON_NULL((1, 2, 3)) AM_ATTR_WARN_UNUSED_RESULT AM_PUBLIC
bool am_ring_enqueue_spsc(struct am_ring *ring, void *buffer, const void *entry, unsigned entry_size);
/** @brief Enqueue an empty entry, returning a pointer to that location
 * @param ring Handle to the ring buffer header
 * @param buffer The buffer itself
 * @param entry_size The size, in bytes, of the entry
 * @return Pointer to the empty entry, or NULL on error
 * @note This enqueue is not visible until committed via am_ring_enqueue_commit_spsc
 */
AM_ATTR_NON_NULL((1, 2)) AM_PUBLIC
void *am_ring_enqueue_reserve_spsc(struct am_ring *ring, void *buffer, unsigned entry_size);
/** @brief Finish the previous call to am_ring_enqueue_reserve_spsc
 * @param ring The ring handle
 */
AM_ATTR_NON_NULL((1)) AM_PUBLIC
void am_ring_enqueue_commit_spsc(struct am_ring *ring);
/** @brief Dequeue an element from the ring buffer
 * @param ring Handle to the ring buffer header
 * @param buffer The buffer itself
 * @param data Pointer to a location where the new entry is memcpy'd
 * @param entry_size Size, in bytes, of the entry
 * @return false if the ring buffer is empty, or true on success
 */
AM_ATTR_NON_NULL((1, 2, 3)) AM_PUBLIC
bool am_ring_dequeue_spsc(struct am_ring *ring, const void *buffer, void *data, unsigned entry_size);

/* MPMC */
AM_ATTR_NON_NULL((1, 2, 3)) AM_ATTR_WARN_UNUSED_RESULT AM_PUBLIC
bool am_ring_enqueue_mpmc(struct am_ring *ring, void *buffer, const void *entry, unsigned entry_size);
AM_ATTR_NON_NULL((1, 2)) AM_PUBLIC
void *am_ring_enqueue_reserve_mpmc(struct am_ring *ring, void *buffer, unsigned entry_size, unsigned *ticket);
AM_ATTR_NON_NULL((1)) AM_PUBLIC
void am_ring_enqueue_commit_mpmc(struct am_ring *ring, unsigned ticket);
AM_ATTR_NON_NULL((1, 2, 3)) AM_PUBLIC
bool am_ring_dequeue_mpmc(struct am_ring *ring, const void *buffer, void *data, unsigned entry_size);

#endif /* ifndef AM_CONCURRENT_RING_BUFFER_H */
