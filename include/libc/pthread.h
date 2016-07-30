#ifndef PTHREAD_H
#define PTHREAD_H

#include <sys/types.h>

/* pthread attributes */

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);

/* multithreading */

int pthread_yield(void);
pthread_t pthread_self(void);
void pthread_exit(void *);
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);

/* mutex */

#define PTHREAD_MUTEX_INITIALIZER (-1)

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

#endif /* !PTHREAD_H */
