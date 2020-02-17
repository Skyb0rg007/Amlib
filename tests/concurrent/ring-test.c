
#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include "am/concurrent/ring_buffer.h"
#include "am/threads.h"

#define SIZE (1 << 8)

static struct am_ring ring;
static int buffer[SIZE];

int consumer(void *ud)
{
    int i;
    int acc = 0;
    int id = (intptr_t)ud;

    for (i = 0; i < 200; i++) {
        int x;
        bool ret;
        ret = am_ring_dequeue_spsc(&ring, buffer, &x, sizeof(int));
        if (ret) {
            printf("Consumer %d: got %d\n", id, x);
            acc++;
        } else {
            i--;
        }
    }
    return acc;
}

int producer(void *ud)
{
    int i;
    int id = (intptr_t)ud;

    for (i = 0; i < 200; i++) {
        bool ret;
        ret = am_ring_enqueue_spsc(&ring, buffer, &i, sizeof(int));
        if (ret) {
            printf("Producer %d: sent %d\n", id, i);
        } else {
            i--;
        }
    }       
    return 0;
}

int main(void)
{
    am_thread t_prod[2], t_cons[2];
    int acc1, acc2;

    am_ring_init(&ring, SIZE);
    am_thread_create(&t_prod[1], producer, (void *)1);
    am_thread_create(&t_prod[2], producer, (void *)2);
    am_thread_create(&t_cons[1], consumer, (void *)1);
    am_thread_create(&t_cons[2], consumer, (void *)2);
    am_thread_join(t_prod[1], NULL);
    am_thread_join(t_prod[2], NULL);
    am_thread_join(t_cons[1], &acc1);
    am_thread_join(t_cons[2], &acc2);

    printf("Final accum: %d, %d\n", acc1, acc2);
    return 0;
}
