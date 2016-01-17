#include "thread.h"
#include "sys/pthread.h"

void *sys_vect[] = {
	thread_yield,
	thread_self,
	thread_exit,
	pthread_create_1
};
