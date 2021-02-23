#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <list.h>
#include <stdbool.h>
#include <threads/thread.h>
//#include <threads/lock.h>

/* Semaphore */
struct semaphore {
    unsigned value; /* Current value */
    struct list waiters; /* List of waiting threads */
    struct list_elem elem; /* condvar waiters list elem  */
};


void semaphore_init(struct semaphore *, unsigned value);
void semaphore_down(struct semaphore *);
void semaphore_down_lock(struct semaphore *sema, struct thread *t, struct list_elem elem);//, struct lock *l);
bool semaphore_try_down(struct semaphore *);
void semaphore_up(struct semaphore *);
void semaphore_self_test(void);

#endif /* UCSC CSE130 */
