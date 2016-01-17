#include "thread.h"
#include "pthread.h"
#include "sched-rr.h"

int pthread_create_1(/* __user */ pthread_t *thread, /* const pthread_attr_t *attr, */
		void *(*start_routine)(void *), void *arg)
{
	struct thread_info *thread_info;

	/* FIXME: We must check all addresses of user-supplied pointers, they must belong
	   to this process user-space.    */
	if ((thread_info = thread_create(start_routine, arg, THREAD_PRIV_USER)) == NULL)
		return -1;
	*thread = (pthread_t) thread_info->ti_id;

	sched_rr_add(thread_info);

	return 0;
}