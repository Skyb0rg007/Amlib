
#ifndef AM_DATA_BITMAP_H
#define AM_DATA_BITMAP_H 1

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "am/macros.h"

#define DIV_ROUND_UP(n, d)   (((n) + (d) - 1) / (d))
#define BITS_TO_ULONGS(bits) DIV_ROUND_UP(bits, sizeof(unsigned long) * 8)
#define BITS_PER_LONG

#define am_bitmap(nbits) \
    struct { unsigned long _bits[BITS_TO_ULONGS(nbits)] }

/* Operations */
/* bm = 0 */
#define am_bitmap_zero(bm) \
    ((void)(memset((bm)->_bits, 0x0, sizeof((bm)->_bits))))
/* bm = ~0 */
#define am_bitmap_fill(bm) \
    ((void)(memset((bm)->_bits, 0xff, sizeof((bm)->_bits))))
/* dst = src */
#define am_bitmap_copy(dst, src) \
    ((void)(memcpy((dst)->_bits, (src)->_bits, AM_MIN(sizeof((dst)->_bits), sizeof((src)->_bits)))))
/* dst = src1 & src2 */
#define am_bitmap_and(dst, src1, src2) \

static AM_INLINE void am__bitmap_and(unsigned *dst, size_t dst_len, unsigned *src1, size_t src1_len, unsigned *src2, size_t src2_len)
{
    unsigned k, lim = bits / BITS_PER_LONG, res = 0;
}

#endif /* ifndef AM_DATA_BITMAP_H */
