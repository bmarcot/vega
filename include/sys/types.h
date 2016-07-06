#ifndef SYS_TYPES_H
#define SYS_TYPES_H

/* http://pubs.opengroup.org/onlinepubs/009696699/basedefs/sys/types.h.html */

/* Used for file sizes. */
typedef signed int off_t;

/* Used to identify a thread attribute object. */
typedef struct {
	int    flags;
	size_t stacksize;
	/* void               *__stackaddr; */
	/* void		      (*__exitfunc)(void *status); */
	/* int		      __policy; */
	/* struct sched_param __param; */
	/* unsigned	      __guardsize; */
} pthread_attr_t;

/* Used for mutexes. */
typedef int pthread_mutex_t;

/* Used to identify a thread. */
typedef unsigned int pthread_t;

/* Used for sizes of objects. */
typedef unsigned int size_t;

/* Used for a count of bytes or an error indication. */
typedef signed int ssize_t;

#endif /* !SYS_TYPES_H */
