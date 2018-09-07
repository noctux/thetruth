#ifndef __GUARD_TICKETLOCK_H_
#define __GUARD_TICKETLOCK_H_

#include <pthread.h>

typedef struct ticket_lock {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    volatile unsigned long queue_head, queue_tail;
} ticket_lock_t;

int ticket_lock_init(ticket_lock_t *ticket);
void ticket_lock_destroy(ticket_lock_t *ticket);
void ticket_lock(ticket_lock_t *ticket);
void ticket_unlock(ticket_lock_t *ticket);

#endif
