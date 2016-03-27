#include "sys/pthread.h"
#include "mutex.h"

#define SYS_MAX 16

void *sys_vect[SYS_MAX] = {
	/* multithreading */
	pthread_yield_1,
	pthread_self_1,
	pthread_exit_1,
	pthread_create_1,

	/* mutex */
	mutex_lock,
	mutex_unlock
};

int sysvect_register(unsigned ix, void *(*fn)())
{
	if (ix >= SYS_MAX)
		return -1;
	sys_vect[ix] = fn;

	return 0;
}
