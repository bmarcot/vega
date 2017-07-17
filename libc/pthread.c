#include <pthread.h>
#include <stddef.h>
#include <string.h>

#include "linux/list.h"

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
	attr->stacksize = stacksize;

	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
	*stacksize = attr->stacksize;

	return 0;
}

int pthread_attr_init(pthread_attr_t *attr)
{
	attr->stackaddr_set = 0;
	attr->stacksize = 512;

	return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	(void)attr;

	*mutex = PTHREAD_MUTEX_INITIALIZER;

	return 0;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
	(void)cond, (void)attr;

	return 0;
}

/* syscall wrappers */

#include <kernel/syscalls.h>

#include "vega/syscalls.h"

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	return do_syscall2((void *)cond, (void *)mutex, SYS_PTHREAD_COND_WAIT);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
	return do_syscall1((void *)cond, SYS_PTHREAD_COND_SIGNAL);
}
