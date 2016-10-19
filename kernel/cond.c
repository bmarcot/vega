/* kernel/condition.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/mutex.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/list.h"

/* The cond variable storage is provided by the user, and the kernel must check
 * the addresses provided in the struct. See issue #8 (LDRT/STRT).  */
int sys_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	CURRENT_THREAD_INFO(cur_thread);

	/* if (cond->owner != cur_thread) { */
	/* 	errno = EINVAL; */
	/* 	return -1; */
	/* } */
	list_add_tail(&cur_thread->ti_q, &cond->waitq);
	cur_thread->ti_state = THREAD_STATE_BLOCKED;
	sys_pthread_mutex_unlock(mutex);

	//XXX: yield IS NOT mandatory; previous unlock may have rescheduled!
	//__pthread_yield();

	/* contend for the lock */
	sys_pthread_mutex_lock(mutex);

	return 0;
}

int sys_pthread_cond_signal(pthread_cond_t *cond)
{
	struct thread_info *waiter;
	CURRENT_THREAD_INFO(cur_thread);

	waiter = list_first_entry_or_null(&cond->waitq, struct thread_info, ti_q);
	if (waiter == NULL)
		return 0;
	list_del(&waiter->ti_q);
	sched_enqueue(waiter);
	if (waiter->ti_priority >= cur_thread->ti_priority) {
		sched_enqueue(cur_thread);
		sched_elect(SCHED_OPT_NONE);
	}

	return 0;
}
