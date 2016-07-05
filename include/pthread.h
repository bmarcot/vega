#ifndef PTHREAD_H
#define PTHREAD_H

#include "linux/types.h"

//FIXME: this is the lib, move to a lib folder

typedef unsigned int pthread_t;
typedef int pthread_mutex_t;

typedef struct {
	int    flags;
	size_t stacksize;
	void   *__stackaddr;
	/* void		    (*__exitfunc)(void *status); */
	/* int		    __policy; */
	/* struct sched_param  __param; */
	/* unsigned	    __guardsize; */
} pthread_attr_t;

#define PTHREAD_ATTR_STACKSIZE 1

/* create and get attributes */

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);

/* multithreading */

int pthread_yield(void);
pthread_t pthread_self(void);
void pthread_exit(void *);
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);

/* mutex */

#define PTHREAD_MUTEX_INITIALIZER (-1)

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

#endif /* !PTHREAD_H */
