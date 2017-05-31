/*
 * kernel/cond.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/mutex.h>
#include <kernel/sched.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/list.h"

static LIST_HEAD(cond_head);

static struct task_struct *find_other_thread(pthread_cond_t *cond)
{
	struct task_struct *other;

	list_for_each_entry(other, &cond_head, ti_q) {
		if (other->ti_private == cond)
			return other;
	}

	return NULL;
}

int sys_pthread_cond_wait(pthread_cond_t *cond, kernel_mutex_t *mutex)
{
	struct task_struct *current = current_thread_info()->task;

	current->ti_private = cond;
	current->ti_state = THREAD_STATE_BLOCKED;
	list_add_tail(&current->ti_q, &cond_head);
	sys_pthread_mutex_unlock(mutex);

	/* contend for the lock */
	sys_pthread_mutex_lock(mutex);

	return 0;
}

int sys_pthread_cond_signal(pthread_cond_t *cond)
{
	struct task_struct *other = find_other_thread(cond);
	if (other == NULL)
		return 0;
	list_del(&other->ti_q);
	sched_enqueue(&other->stack->thread_info);

	struct task_struct *current = current_thread_info()->task;
	if (other->ti_priority >= current->ti_priority) {
		sched_enqueue(&current->stack->thread_info);
		sched_elect(SCHED_OPT_NONE);
	}

	return 0;
}
