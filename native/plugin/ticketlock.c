#include <pthread.h>

#include "ticketlock.h"

// Adapted from https://stackoverflow.com/a/12703543
//  under CC Attribution Sharealike by "caf"

#define TICKET_LOCK_INITIALIZER { PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER }

int ticket_lock_init(ticket_lock_t *ticket)
{
	if(!pthread_cond_init(&(ticket->cond), NULL)) {
		return -1;
	}
	if(!pthread_mutex_init(&(ticket->mutex), NULL)) {
		return -1;
	}
	ticket->queue_head = 0;
	ticket->queue_tail = 0;
	return 0;
}

void ticket_lock_destroy(ticket_lock_t *ticket)
{
	pthread_cond_destroy(&(ticket->cond));
	pthread_mutex_destroy(&(ticket->mutex));
}

void ticket_lock(ticket_lock_t *ticket)
{
    unsigned long queue_me;

    pthread_mutex_lock(&ticket->mutex);
    queue_me = ticket->queue_tail++;
    while (queue_me != ticket->queue_head)
    {
        pthread_cond_wait(&ticket->cond, &ticket->mutex);
    }
    pthread_mutex_unlock(&ticket->mutex);
}

void ticket_unlock(ticket_lock_t *ticket)
{
    pthread_mutex_lock(&ticket->mutex);
    ticket->queue_head++;
    pthread_cond_broadcast(&ticket->cond);
    pthread_mutex_unlock(&ticket->mutex);
}
