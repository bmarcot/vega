#include "thread.h"
#include "sys/pthread.h"

void *sys_vect[] = {
	pthread_yield_1,
	pthread_self_1,
	pthread_exit_1,
	pthread_create_1
};
