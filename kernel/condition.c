/* mutex.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/mutex.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/list.h"

// user provides the storage (and kernel must do checks)

int __pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	// mutex is locked

	// unlock mutex and add current thread to a list a thread blocking
	// on the cond. a list ?


	// create condition list
	CURRENT_THREAD_INFO(cur_thread);
	//list_add(&cur->q, &cond->list);
	list_add_tail(&cur_thread->ti_q, &cond->waitq);

	//IDEA: set priority as min, such that pthread_unlock puts so;ething else
	// on the CPU?
	// or change state ??
	cur_thread->ti_state = THREAD_STATE_BLOCKED;

	// 1. unlock mutex

	__pthread_mutex_unlock(mutex);

	//XXX: yield IS NOT mandatory; previous unlock may have rescheduled!
	//__pthread_yield();

	// 2. contend for the lock
	__pthread_mutex_lock(mutex);

	return 0;
}

int __pthread_cond_signal(pthread_cond_t *cond)
{
	struct thread_info *waiter;
	CURRENT_THREAD_INFO(cur_thread);

	waiter = list_first_entry_or_null(&cond->waitq, struct thread_info, ti_q);

	// nothing to wakeup
	if (waiter == NULL)
		return 0;

	list_del(&waiter->ti_q);
	sched_enqueue(waiter);

	// si prio plus forte...
	if (waiter->ti_priority >= cur_thread->ti_priority) {
		sched_enqueue(cur_thread);
		sched_elect(SCHED_OPT_NONE);
	}

	return 0;
}

/* int pthread_cond_destroy(pthread_cond_t *cond); */

/* pthread_cond_t cond = PTHREAD_COND_INITIALIZER; */
