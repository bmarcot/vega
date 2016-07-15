/* mutex.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdio.h>
#include <stdlib.h>

#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/types.h"
#include "linux/list.h"
#include "kernel.h"

struct mutex {
	atomic_t *m_lock;
	struct list_head waitq;    /* shared list between mutex's waitq and active runq */
	struct list_head list;     /* list of all mutex in the system */
};

/* global list of mutex structs */
LIST_HEAD(mutexes);

int mutex_lock(atomic_t /* __user  */ *lock)
{
	printk("mutex: locking %p (val=%d)\n", lock, lock->val);

	struct mutex *mutexp;
	CURRENT_THREAD_INFO(threadp);

	/* The lock has been released in the meantime, i.e. in-between the lock
	   checking and the syscall instruction. Acquire the lock and leave.    */
	if (lock->val == -1) {
		lock->val = 0;
		return 0;
	}

	/* Check if a kernel structure has been created previously (i.e. the mutex
	   has already been unsuccessfully locked once). Create a structure if
	   that's the first time the mutex is locked through the slow path.    */
	list_find_entry(mutexp, &mutexes, list, lock, m_lock);
	if (mutexp == NULL) {
		mutexp = malloc(sizeof (struct mutex));    //XXX: kmalloc
		mutexp->m_lock = lock;
		INIT_LIST_HEAD(&mutexp->waitq);
		list_add(&mutexp->list, &mutexes);
		printk("mutex: no mutex struct where found, created %p\n", mutexp);
	}

	lock->val++;    /* data is in the [1, n] range */

	/* We don't loop on the sched_elect() because the only way to pass that
	   barrier (i.e. return from the sched_elect()) is because another thread
	   released the mutex and explicitely put that waiting thread back into
	   runqueue. By design, no other thread can have acquired the mutex in
	   the meantime because the lock value is still positive or equal to 0
	   and they would enter the locking slow path.    */
	list_add_tail(&threadp->ti_list, &mutexp->waitq);
	sched_elect(SCHED_OPT_NONE);

	/* Return to userland with the mutex.     */
	return 0;
}

int mutex_unlock(atomic_t /* __user */ *lock)
{
	struct mutex *mutexp;
	struct thread_info *waiter;

	printk("mutex: unlocking %p (val=%d)\n", lock, lock->val);

	/* we are here because there is at least one waiting thread */

	/* Pop one there in the list of threads waiting for the lock. Add this
	   thread to the runqueue.    */
	list_find_entry(mutexp, &mutexes, list, lock, m_lock);
	if (mutexp == NULL)
		return -1;
	waiter = list_first_entry(&mutexp->waitq, struct thread_info, ti_list);
	list_del(&waiter->ti_list);
	sched_add(waiter);

	if (list_empty(&mutexp->waitq)) {
		/* We could free the structure here. However, there is obvious
		   contentions to get that lock otherwise we wouldn't be here.
		   Let's keep it for now.    */
	}

	lock->val--;

	//XXX: elect iff one thread of high-prio is blocking on that mutex
	CURRENT_THREAD_INFO(current);
	sched_add(current);
	sched_elect(SCHED_OPT_NONE);

	return 0;
}
