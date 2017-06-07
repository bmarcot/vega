#ifndef _SYS_PTHREAD_TYPES_H
#define _SYS_PTHREAD_TYPES_H

typedef unsigned long pthread_mutex_t;
typedef unsigned long pthread_mutexattr_t;
typedef unsigned long pthread_t;
typedef unsigned long pthread_cond_t;
typedef unsigned long pthread_condattr_t;

typedef struct
{
	int           stackaddr_set; //FIXME: Replace with __flags | PA_STACKSET
	void          *stackaddr;
	unsigned long stacksize;
} pthread_attr_t;

#endif /* !_SYS_PTHREAD_TYPES_H */
