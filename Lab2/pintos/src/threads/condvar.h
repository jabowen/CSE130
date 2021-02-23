#ifndef CONDVAR_H
#define CONDVAR_H

#include "threads/semaphore.h"
#include "threads/lock.h"

/* Condition variable */
struct condvar {
    struct list waiters; /* List of semaphore_elems */
};

bool cond_list_less(const struct list_elem* a, const struct list_elem* b, void* aux);
void condvar_init(struct condvar *);
void condvar_wait(struct condvar *, struct lock *);
void condvar_signal(struct condvar *, struct lock *);
void condvar_broadcast(struct condvar *, struct lock *);

#endif /* UCSC CSE130 */
