#include <stdio.h>
#include <stdlib.h>

#include "linux/types.h"
#include "linux/list.h"
#include "thread.h"
#include "sched-rr.h"

struct mutex {
	atomic_t *m_lock;
	struct list_head waitq; // shared list between mutex's waitq and active runq
	struct list_head list;  // list of all mutex in the system
};

/* global list of struct mutex_info */
LIST_HEAD(mutexes);

/* slow-path */
// mutex_lock_slow_path
int mutex_lock(atomic_t /* __user  */ *lock)
{
	printf("mutex: locking %p (val=%d)\n", lock, lock->val);

	struct mutex *mutexp;
	CURRENT_THREAD_INFO(threadp);

	// lock has been released in the meantime of pthread_mutex_lock entry and the syscall
	// acquire the lock and leave
	if (lock->val == -1) {
		lock->val = 0;
		return 0;
	}

	/* Check if a kernel structure has been created previously (i.e. the
	      mutex has already been unsuccessfully locked once). Create a
	         structure if that's the first time the mutex is locked through the
		 slow path.    */
	list_find_entry(mutexp, &mutexes, list, lock, m_lock);
	if (mutexp == NULL) {
		//kmalloc
		mutexp = malloc(sizeof (struct mutex));
		mutexp->m_lock = lock;
		INIT_LIST_HEAD(&mutexp->waitq);
		list_add(&mutexp->list, &mutexes);
		printf("mutex: no mutex struct where found, created %p\n", mutexp);
	}

	/* *data++; // count the number of threads currently waiting for that thread? this counter is reset by an */
	/* // unlock in between 2 locks... should just write *data = 1; */
	/* *data = 1; */
	/* mutexp->counter++; */

	// data is in the [1, n] range
	lock->val++;
	//printf("lock->val = %d\n", lock->val);

	/* Move the thread from the active runqueue to the mutex's waitqueue,
	   and elect the new thread to run on the CPU.    */
	/* do this in a loop because of thread priority. A high prio thread can release have release the mutex
	      and this waiting thread has been added to the runqueue. but that thread is of lower priorit


y
	         and won't run immediately. in the meantime a higher prio thread can have locked the mutex and that
		 thread must go back to the waitq. */

	/* We don't loop on the sched_elect() because the only way to pass that
	   barrier (i.e. return from the sched_elect()) is because another thread
	   released the mutex and explicitely put that waiting thread back into
	   runqueue. By design, no other thread can have acquired the mutex in
	   the meantime because the lock value is still positive or equal to 0
	   and they would enter the locking slow path.    */
	list_move(&threadp->ti_list, &mutexp->waitq);
	sched_rr_elect_reset();

	/* // LOOP ?? not  working id val as counter greater than 1... */
	/* // no need to retry because other thread of hi prio will wait because val greater than 0 */
	/* do { */
	/* 	list_move(&threadp->ti_list, &mutexp->waitq); */

	/* 	//XXX: remove and elect in sched_xxx() */
	/* 	//sched_rr_del(threadp); */
	/* 	sched_rr_elect_first(); */

	/* 	/\* for (;;) *\/ */
	/* 	/\* 	; *\/ */

	/* 	/\* list_move(&threadp->ti_list, &mutexp->waitq); *\/ */
	/* 	/\* printf("  before elect\n"); *\/ */
	/* 	/\* sched_rr_elect(); *\/ */
	/* 	/\* Returning here means lock has been released, threads had been */
	/* 	   moved to runqueue and eventually had been elected.    *\/ */
	/* } while (lock->val > 0); */

	/* mutexp->counter--; */
	/* if (mutexp->counter) */
	/* *data = 1; */
	/* else */
	/* *data = 0; */

	/* Return to userland with the mutex.     */
	return 0;
}


/* extern struct list_head rr_runq; */

int mutex_unlock(atomic_t /* __user */ *lock)
{
	struct mutex *mutexp;

	printf("mutex: unlocking %p (val=%d)\n", lock, lock->val);

	// likewise lock(), threads waiting on the mutex could have been delete in the meantime
	// of the entry so maybe there is no thread to wake-up
	// XXX: does POSIX allow to kill threads this way?

	// we are here because there is at least one waiting thread

	/* Pop one there in the list of threads waiting for the lock. Add this
	   thread to the runqueue.    */
	list_find_entry(mutexp, &mutexes, list, lock, m_lock);
	//printf("uUu found mutex with thread to wake up %p\n", mutexp);
	if (mutexp == NULL)
		return -1;
	struct thread_info *waiter = list_first_entry(&mutexp->waitq, struct thread_info, ti_list);
	//printf("waiter thread is %p\n", waiter);
	//list_move_tail(&mutexp->waitq, &rr_runq);
	//list_move(waiter->ti_list, &rr_runq);
	//list_del(&waiter->ti_list);
	sched_rr_add(waiter);

	if (list_empty(&mutexp->waitq)) {
		/* We could free the structure here. However, there is obvious
		      contentions to get that lock otherwise we wouldn't be here.
		      Let's keep it for now.    */
	}
	//}

	//*data = -1; // release the lock, maybe the thread we woke up get it, maybe some other thread gets it

	// alternatively, consider the lock as a counter, and enter the lock/unlock syscall
	// as soon as at leat one thread have been pushed from the waitq to the runq => no counter
	lock->val--;
	//printf("mutex unlock val %d\n", lock->val);

	/* for (;;) */
	/* 	; */

	//sched_elect(); //XXX: ?? yes if one thread of high-prio is blocking on that mutex

	return 0;
}
