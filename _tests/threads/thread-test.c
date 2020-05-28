
#define _GNU_SOURCE
#include <stdio.h>
#include "am/macros.h"
#include "am/threads.h"

int worker(void *ud)
{
    int i;
    (void)ud;

    for (i = 0; i < 20; i++) {
        printf("Iteration %d\n", i);
    }
    return 0;
}

int main(void)
{
    am_thread t1, t2;
    int ret;

    am_thread_create(&t1, worker, NULL);
    am_thread_create(&t2, worker, NULL);
    am_thread_join(t1, &ret);
    am_thread_join(t2, &ret);
    return ret;
}

