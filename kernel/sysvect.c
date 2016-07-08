#include <pthread.h>

#include <kernel/mutex.h>
#include <kernel/timer.h>

#include "unistd.h"

int pthread_yield_1(void);
pthread_t pthread_self_1(void);
void pthread_exit_1(void *);
int pthread_create_1(/* __user */ pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);

#define SYS_MAX 16

void *syscall_vector[SYS_MAX] = {
	/* multithreading */
	pthread_yield_1,
	pthread_self_1,
	pthread_exit_1,
	pthread_create_1,

	/* mutex */
	mutex_lock,
	mutex_unlock,

	/* timers */
	timer_create_1,

	/* unix standards */
	sysconf_1
};

int sysvect_register(unsigned ix, void *(*fn)())
{
	if (ix >= SYS_MAX)
		return -1;
	syscall_vector[ix] = fn;

	return 0;
}
