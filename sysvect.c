#include "thread.h"
#include "sys/pthread.h"

#define SYS_MAX 16

void *sys_vect[SYS_MAX] = {
	pthread_yield_1,
	pthread_self_1,
	pthread_exit_1,
	pthread_create_1
};

int sysvect_register(unsigned ix, void *(*fn)())
{
	if (ix >= SYS_MAX)
		return -1;
	sys_vect[ix] = fn;

	return 0;
}
