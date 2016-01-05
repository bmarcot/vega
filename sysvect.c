#include "thread.h"

int sc_null_1(void);

void *sys_vect[] = {
	sc_null_1,
	thread_yield,
	thread_self,
	thread_exit
};
