
#ifndef AM_DATA_LIST_H
#define AM_DATA_LIST_H 1

#include <stddef.h>
#include <stdbool.h>
#include "am/macros.h"

/** @brief List head + intrusive list entry */
struct am_list {
    struct am_list *prev, /**< Pointer to the previous list element */
                   *next; /**< Pointer to the next list element */
};

#define AM_LIST_HEAD_INITIALIZER(name) { &(name), &(name) }

AM_ATTR_NON_NULL((1)) static AM_INLINE
void am_list_head_init(struct am_list *head);
AM_ATTR_NON_NULL((1, 2)) static AM_INLINE
void am_list_add(struct am_list *new, struct am_list *head);
AM_ATTR_NON_NULL((1, 2)) static AM_INLINE
void am_list_add_tail(struct am_list *new, struct am_list *head);

AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_list_head_init(struct am_list *head)
{
    head->next = head;
    head->prev = head;
}

AM_ATTR_NON_NULL((1, 2, 3))
static AM_INLINE
void am__list_add(
        struct am_list *new,
        struct am_list *prev,
        struct am_list *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_list_add(struct am_list *new, struct am_list *head)
{
    am__list_add(new, head, head->next);
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_list_add_tail(struct am_list *new, struct am_list *head)
{
    am__list_add(new, head->prev, head);
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am__list_del(struct am_list *prev, struct am_list *next)
{
    next->prev = prev;
    prev->next = next;
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
void am_list_del(struct am_list *entry)
{
    am__list_del(entry->prev, entry->next);
    am_list_head_init(entry);
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_list_replace(struct am_list *old, struct am_list *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
    am_list_head_init(old);
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_list_swap(struct am_list *entry1, struct am_list *entry2)
{
    struct am_list *pos = entry2->prev;

    am_list_del(entry2);
    am_list_replace(entry1, entry2);
    if (pos == entry1) {
        pos = entry2;
    }
    am_list_add(entry1, pos);
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_list_move(struct am_list *list, struct am_list *head)
{
    am_list_del(list);
    am_list_add(list, head);
}

AM_ATTR_NON_NULL((1, 2))
static AM_INLINE
void am_list_move_tail(struct am_list *list, struct am_list *head)
{
    am_list_del(list);
    am_list_add_tail(list, head);
}

AM_ATTR_NON_NULL((1))
static AM_INLINE
bool am_list_is_empty(struct am_list *head)
{
    return head->next == head;
}

#define am_list_foreach(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define am_list_foreach_safe(pos, tmp, head) \
    for (pos = (head)->next, tmp = pos->next; pos != (head); pos = tmp, tmp = pos->next)

#endif /* ifndef AM_DATA_LIST_H */
