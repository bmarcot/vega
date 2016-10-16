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
int sys_pthread_mutex_lock(pthread_mutex_t *mutex)
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

	cur_thread->ti_state = THREAD_STATE_BLOCKED;

	sched_elect(SCHED_OPT_NONE);

	return 0;
}

/* The release procedure fetches the first thread in the waiting list.  The user
 * pthread_mutex_unlock() does a syscall iff there is at least one thread in the
 * waiting list, so the waiting list cannot be empty.  The procedure can be
 * called from the kernel; such that the waiting list may be empty.  Whatever
 * the call site, only the thread owning the mutex shall release it.  A waiting
 * thread runs immediately on the CPU if its priority is greater-equal than the
 * current thread's priority.  */

extern void *syscall_return;

int sys_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	struct thread_info *waiter;
	CURRENT_THREAD_INFO(cur_thread);

	waiter = list_first_entry_or_null(&mutex->waitq, struct thread_info, ti_q);
	if (waiter != NULL) {
		list_del(&waiter->ti_q);
		sched_enqueue(waiter);
	} else if (__builtin_return_address(0) == &syscall_return) {
		//XXX: list cannot be empty when calling from userland
		return -1;
	}
	mutex->val--;

	if (cur_thread->ti_state == THREAD_STATE_BLOCKED) {
		sched_elect(SCHED_OPT_NONE);
	} else if (waiter && (cur_thread->ti_priority <= waiter->ti_priority)) {
		sched_enqueue(cur_thread);
		sched_elect(SCHED_OPT_NONE);
	}

	return 0;
}
