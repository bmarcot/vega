/*
 * kernel/mutex.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdlib.h>
#include <sys/types.h>

#include <kernel/kernel.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/list.h"

LIST_HEAD(mutex_head);

struct mutex_info {
	pthread_mutex_t  *mutex;
	struct list_head waitq;
	struct list_head list;
};

static struct mutex_info *find_mutex_info_by_addr(pthread_mutex_t *mutex)
{
	struct mutex_info *mutex_info;

	list_for_each_entry(mutex_info, &mutex_head, list) {
		if (mutex_info->mutex == mutex)
			return mutex_info;
	}

	return NULL;
}

/* the thread owns the mutex on return */
int sys_pthread_mutex_lock(pthread_mutex_t *mutex)
{
	mutex->val++;

	/* the lock has been released between the mutex test and the syscall */
	if (!mutex->val)
		return 0;

	struct mutex_info *mutex_info = find_mutex_info_by_addr(mutex);
	if (mutex_info == NULL) {
		mutex_info = malloc(sizeof(struct mutex_info));
		if (mutex_info == NULL)
			return -1;
		mutex_info->mutex = mutex;
		INIT_LIST_HEAD(&mutex_info->waitq);
		list_add_tail(&mutex_info->list, &mutex_head);
	}

	CURRENT_THREAD_INFO(curr_thread);
	list_add_tail(&curr_thread->ti_q, &mutex_info->waitq);
	curr_thread->ti_state = THREAD_STATE_BLOCKED;
	sched_elect(SCHED_OPT_NONE);

	return 0;
}

int sys_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	struct thread_info *waiter = NULL;

	if (--mutex->val >= 0) {
		struct mutex_info *mutex_info = find_mutex_info_by_addr(mutex);
		if (mutex_info == NULL)
			return -1;
		waiter = list_first_entry_or_null(&mutex_info->waitq,
						struct thread_info, ti_q);
		list_del(&waiter->ti_q);
		sched_enqueue(waiter);
		if (!mutex->val)
			free(mutex_info);
	}

	CURRENT_THREAD_INFO(curr_thread);
	if (curr_thread->ti_state == THREAD_STATE_BLOCKED) {
		sched_elect(SCHED_OPT_NONE);
	} else if (waiter && (curr_thread->ti_priority <= waiter->ti_priority)) {
		sched_enqueue(curr_thread);
		sched_elect(SCHED_OPT_NONE);
	}

	return 0;
}
