/*
 * kernel/mutex.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/kernel.h>
#include <kernel/mutex.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <asm/current.h>

#include "linux/list.h"

static LIST_HEAD(mutex_head);

/* The thread owns the mutex on return. We also check the case when the lock
 * has been released between the test of the mutex and this syscall. */
int sys_pthread_mutex_lock(kernel_mutex_t *mutex)
{
	mutex->val++;
	if (!mutex->val)
		return 0;

	current->ti_private = mutex;
	current->ti_state = THREAD_STATE_BLOCKED;
	list_add_tail(&current->ti_q, &mutex_head);
	sched_elect(SCHED_OPT_NONE);

	return 0;
}

static struct task_struct *find_first_blocking_thread(kernel_mutex_t *mutex)
{
	struct task_struct *thread;

	list_for_each_entry(thread, &mutex_head, ti_q) {
		if (thread->ti_private == mutex)
			return thread;
	}

	return NULL;
}

int sys_pthread_mutex_unlock(kernel_mutex_t *mutex)
{
	struct task_struct *waiter = NULL;

	mutex->val--;
	if (mutex->val >= 0) {
		waiter = find_first_blocking_thread(mutex);
		if (waiter == NULL) {
			printk("[mutex_unlock] No blocking threads for mutex=<%p>\n",
				mutex);
			return -1;
		}
		list_del(&waiter->ti_q);
		sched_enqueue(waiter);
	}

	if (current->ti_state == THREAD_STATE_BLOCKED) {
		sched_elect(SCHED_OPT_NONE);
	} else if (waiter && (current->ti_priority <= waiter->ti_priority)) {
		sched_enqueue(current);
		sched_elect(SCHED_OPT_NONE);
	}

	return 0;
}
