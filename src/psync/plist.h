/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2014 Anton Titov <anton@pcloud.com>
 * (c) 2013-2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PLIST_H_
#define PCLOUD_PSYNC_PLIST_H_

#include <stddef.h>

#include "pcloudcc/compat/compiler.h"

typedef struct psync_list_ {
  struct psync_list_ *next;
  struct psync_list_ *prev;
} psync_list;

typedef int (*psync_list_compare)(const psync_list *, const psync_list *);

#define psync_list_init(l) \
  do {                     \
    (l)->next = (l);       \
    (l)->prev = (l);       \
  } while (0)

#define PSYNC_LIST_STATIC_INIT(l) {&(l), &(l)}

#define psync_list_isempty(l) ((l)->next == (l))
#define psync_list_is_head(l, e) ((l)->next == (e))
#define psync_list_is_tail(l, e) ((l)->prev == (e))

static inline void psync_list_add_between(psync_list *l1, psync_list *l2,
                                          psync_list *a) {
  a->next = l2;
  a->prev = l1;
  l1->next = a;
  l2->prev = a;
}

#define psync_list_add_head(l, a) psync_list_add_between(l, (l)->next, a)
#define psync_list_add_tail(l, a) psync_list_add_between((l)->prev, l, a)
#define psync_list_add_before(e, a) psync_list_add_between((e)->prev, e, a)
#define psync_list_add_after(e, a) psync_list_add_between(e, (e)->next, a)

#define psync_list_del(a)        \
  do {                           \
    (a)->next->prev = (a)->prev; \
    (a)->prev->next = (a)->next; \
  } while (0)

#define psync_list_element(a, t, n) ((t *)((char *)(a) - offsetof(t, n)))

#define psync_list_for_each(a, l) for (    \
    (a) = (l)->next;                       \
    psync_prefetch((a)->next), (a) != (l); \
    (a) = (a)->next                        \
  )

#define psync_list_for_each_safe(a, b, l) for ( \
    (a) = (l)->next, (b) = (a)->next;           \
    psync_prefetch(b), (a) != (l);              \
    (a) = (b), (b) = (b)->next                  \
  )

#define psync_list_for_each_element(a, l, t, n) for ( \
    (a) = psync_list_element((l)->next, t, n);        \
    psync_prefetch((a)->n.next), &(a)->n != (l);      \
    (a) = psync_list_element((a)->n.next, t, n)       \
  )

#define psync_list_for_each_element_call(l, t, n, c) \
  do {                                               \
    psync_list *___tmpa, *___tmpb;                   \
    psync_list_for_each_safe(___tmpa, ___tmpb, l)    \
      c(psync_list_element(___tmpa, t, n));          \
  } while (0)

static inline psync_list *psync_list_remove_head(psync_list *l) {
  l = l->next;
  psync_list_del(l);
  return l;
}

#define psync_list_remove_head_element(l, t, n) \
  psync_list_element(psync_list_remove_head(l), t, n)

void psync_list_sort(psync_list *l, psync_list_compare cmp);
void psync_list_extract_repeating(
    psync_list *l1,
    psync_list *l2,
    psync_list *extracted1,
    psync_list *extracted2,
    psync_list_compare cmp
);

#endif  /* PCLOUD_PSYNC_PLIST_H_ */
