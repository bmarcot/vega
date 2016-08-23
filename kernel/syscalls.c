#include <pthread.h>

#include <kernel/cond.h>
#include <kernel/mutex.h>
#include <kernel/timer.h>

int sys_pthread_yield(void);
pthread_t sys_pthread_self(void);
void sys_pthread_exit(void *);
int sys_pthread_create(/* __user */ pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);
void sys_pthread_join(pthread_t, void **);
int sys_timer_create(unsigned int msecs);
long sys_sysconf(int name);

void __msleep(unsigned int);

#define SYS_MAX 16

void *syscall_vector[SYS_MAX] = {
	/* multithreading */
	sys_pthread_yield,
	sys_pthread_self,
	sys_pthread_exit,
	sys_pthread_create,

	/* mutex */
	__pthread_mutex_lock,
	__pthread_mutex_unlock,

	/* timers */
	sys_timer_create,

	/* unix standards */
	sys_sysconf,

	sys_pthread_join,

	/* condition variable */
	__pthread_cond_signal,
	__pthread_cond_wait,

	__msleep,
};

int syscall_register(unsigned ix, void *(*fn)())
{
	if (ix >= SYS_MAX)
		return -1;
	syscall_vector[ix] = fn;

	return 0;
}
