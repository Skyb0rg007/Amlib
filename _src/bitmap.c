
#include "am/data/bitmap.h"
#include <stdlib.h>

#define DIV_ROUND_UP(n, d)   (((n) + (d) - 1) / (d))
#define BITS_TO_ULONGS(bits) DIV_ROUND_UP(bits, sizeof(unsigned long) * 8)

bool am_bitmap_alloc(struct am_bitmap *bitmap, unsigned nbits)
{
    bitmap->_bits = calloc(BITS_TO_ULONGS(nbits), sizeof(unsigned long));
    return bitmap->_bits != NULL;
}
void am_bitmap_free(struct am_bitmap *bitmap)
{
    free(bitmap->_bits);
    bitmap->_bits = NULL;
}

