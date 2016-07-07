#include <string.h>
#include <ucontext.h>
#include <sys/resource.h>

#include "thread.h"
#include "pthread.h"
#include "sched-rr.h"
#include "utils.h"

static ucontext_t main_context, pthread_context;
static unsigned int ctx_stack[256];
static int create_ret_code;

int pthread_yield_1(void)
{
	return thread_yield();
}

pthread_t pthread_self_1(void)
{
	return (pthread_t) thread_self();
}

void pthread_exit_1(void *retval)
{
	thread_exit(retval);
}

static int pthread_create_2(/* __user */ pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
	struct thread_info *thread_info;
	struct rlimit stacklimit;
	size_t stacksize;

	/* get the thread's stack size */
	getrlimit(RLIMIT_STACK, &stacklimit);
	if (attr) {
		stacksize = MIN(attr->stacksize, stacklimit.rlim_max);
	} else {
		stacksize = stacklimit.rlim_cur;
	}

	/* FIXME: We must check all addresses of user-supplied pointers, they must belong
	   to this process user-space.    */
	if ((thread_info = thread_create(start_routine, arg, THREAD_PRIV_USER, stacksize)) == NULL) {
		create_ret_code = -1;
		return -1;
	}
	*thread = (pthread_t) thread_info->ti_id;

	sched_rr_add(thread_info);

	return 0;
}

int pthread_create_1(/* __user */ pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
	/* link the current context to the print context */
	pthread_context.uc_link = &main_context;
	pthread_context.uc_stack.ss_sp = &ctx_stack[256];

	/* pass the arguments to the new context, and swap */
	makecontext(&pthread_context, pthread_create_2, 4, thread, attr, start_routine,
		arg);
	swapcontext(&main_context, &pthread_context);

	return create_ret_code;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
	if (attr == NULL)
		return -1;
	attr->stacksize = stacksize;

	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
	if (attr == NULL)
		return -1;
	*stacksize = attr->stacksize;

	return 0;
}

const pthread_attr_t pthread_attr_default = {
	.flags = 0,
	.stacksize = 1024
};

int pthread_attr_init(pthread_attr_t *attr)
{
	if (attr == NULL)
		return -1;
	memcpy(attr, &pthread_attr_default, sizeof(pthread_attr_t));

	return 0;
}
