/* mutex.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/list.h"
#include "kernel.h"

/* The acquire procedures adds the current thread to the waiting list for that
 * mutex.  When the syscall returns, the user thread owns the mutex.  */
int __pthread_mutex_lock(pthread_mutex_t *mutex)
{
	CURRENT_THREAD_INFO(cur_thread);

	/* The lock has been released in the meantime, i.e. in-between the lock
	   checking and the syscall instruction. Acquire the lock and leave.    */
	if (mutex->val == -1) {
		mutex->val = 0;
		return 0;
	}

	mutex->val++;    /* data is in the [1, n] range */

	/* We don't loop on the sched_elect() because the only way to pass that
	   barrier (i.e. return from the sched_elect()) is because another thread
	   released the mutex and explicitely put that waiting thread back into
	   runqueue. By design, no other thread can have acquired the mutex in
	   the meantime because the lock value is still positive or equal to 0
	   and they would enter the locking slow path.    */
	list_add_tail(&cur_thread->ti_q, &mutex->waitq);
	sched_elect(SCHED_OPT_NONE);

	return 0;
}

/* The release procedure fetches the first thread in the waiting list.  The user
 * pthread_mutex_unlock() does a syscall iff there is at least one thread in the
 * waiting list, so the waiting list cannot be empty.  The waiting thread runs
 * immediately on the CPU if its priority is greater-equal than the current
 * thread's priority.  */
int __pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	struct thread_info *waiter;
	CURRENT_THREAD_INFO(cur_thread);

	waiter = list_first_entry_or_null(&mutex->waitq, struct thread_info, ti_q);
	if (waiter == NULL) {
		//XXX: list should not be empty
		return -1;
	}
	list_del(&waiter->ti_q);
	sched_enqueue(waiter);
	mutex->val--;
	if (waiter->ti_priority >= cur_thread->ti_priority) {
		sched_enqueue(cur_thread);
		sched_elect(SCHED_OPT_NONE);
	}

	return 0;
}
