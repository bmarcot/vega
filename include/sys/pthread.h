#ifndef SYS_PTHREAD_H
#define SYS_PTHREAD_H

#include "../pthread.h"

int pthread_create_1(/* __user */ pthread_t *thread, /* const pthread_attr_t *attr, */
		void *(*start_routine)(void *), void *arg);

#endif /* !SYS_PTHREAD_H */
