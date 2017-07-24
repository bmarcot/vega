#ifndef _SYS_PTHREAD_TYPES_H
#define _SYS_PTHREAD_TYPES_H

/* thread structs */

typedef unsigned long pthread_t;

typedef struct {
	int           stackaddr_set;
	void          *stackaddr;
	unsigned long stacksize;
} pthread_attr_t;

/* mutex structs */

typedef struct {
	int __lock;
} pthread_mutex_t;

typedef unsigned long pthread_mutexattr_t;

/* condvar structs */

typedef struct {
	int          __lock;
	int          __futex;
	unsigned int __nwaiters;
} pthread_cond_t;

typedef unsigned long pthread_condattr_t;

#endif /* !_SYS_PTHREAD_TYPES_H */
