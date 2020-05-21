
#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include "am/alloc.h"
#include "am/threads.h"
#include "am/utils.h"

#define CHUNK_SIZE 120

static
void report(struct timespec *start, struct timespec *stop)
{
    if (stop->tv_nsec < start->tv_nsec) {
        printf("%ld.%3.3ld", stop->tv_sec - start->tv_sec - 1, (stop->tv_nsec - start->tv_nsec + 1000000000) / 1000);
    } else {
        printf("%ld.%3.3ld", stop->tv_sec - start->tv_sec, (stop->tv_nsec - start->tv_nsec) / 1000);
    }
}

/* static AM_ATTR_NEVER_INLINE
void test_alloc(const char *alloc_name, struct am_alloc *alloc)
{
    struct timespec now = {0}, then = {0};
    static void *ptrs[2000];
    am_gettimespec(&now);
    {
        int i;
        for (i = 0; i < 2000; i++) {
            ptrs[i] = am_malloc(alloc, CHUNK_SIZE);
            assert(ptrs[i] != NULL);
        }
    }
    am_gettimespec(&then);
    printf("%-8s ", alloc_name);
    report(&now, &then);
    puts("\n");
    if (strcmp(alloc_name, "default") == 0) {
        int i;
        for (i = 0; i < 2000; i++) {
            am_free(alloc, ptrs[i], CHUNK_SIZE);
        }
    }
} */

int main(void)
{
    /*
    struct am_alloc_stack stack;
    struct am_alloc_pool  pool;
    struct am_alloc deflt;

    am_alloc_init_stack(&stack);
    test_alloc("stack", (struct am_alloc *)&stack);
    am_alloc_destroy_stack(&stack);

    am_alloc_init_pool(&pool, CHUNK_SIZE);
    test_alloc("pool", (struct am_alloc *)&pool);
    am_alloc_destroy_pool(&pool);

    am_alloc_init_default(&deflt);
    test_alloc("default", &deflt);
 */
    return 0;
}
