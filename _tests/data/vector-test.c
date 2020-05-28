
#include <stdio.h>
#include <inttypes.h>
#include "am/data/_vector.h"

typedef struct { int a, b; } asdf;

char* itoa(intmax_t value, char* result, int base) {
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

void pr_bin(uintmax_t n) {
    char buf[33];
    itoa(n, buf, 2);
    printf("%s\n", buf);
}

int main(void)
{
    int i;
    asdf *it;
    am_vector(asdf) v;
    am_vector_init(&v);

    uint8_t short_mask = 0x80;
    uintptr_t long_mask = ~((uintptr_t)(~(uintptr_t)0) >> 1);
    printf("Short: %#"PRIx8"\n", short_mask);
    printf("Long:  %#"PRIxPTR"\n", long_mask);
    printf("Short: 0b"); pr_bin(short_mask);
    printf("Long:  0b"); pr_bin(short_mask);
    return 0;

    am_vector_push_back(&v, ((asdf){ 1, 1 }));
    am_vector_push_back(&v, ((asdf){ 2, 1 }));
    am_vector_push_back(&v, ((asdf){ 3, 1 }));
    am_vector_push_back(&v, ((asdf){ 4, 1 }));

    am_vector_insert(&v, 3, ((asdf){ 100, 100 }));
    am_vector_insert(&v, 2, ((asdf){ 100, 200 }));

    am_vector_shrink_to_fit(&v);

    i = 0;
    am_vector_foreach(it, &v) {
        printf("[%d]: (%d, %d)\n", i, it->a, it->b);
        i++;
    }

    printf("\n");
    i = am_vector_size(&v);
    am_vector_foreach_rev(it, &v) {
        i--;
        printf("[%d]: (%d, %d)\n", i, it->a, it->b);
    }

    am_vector_free(&v);

    return 0;
}

