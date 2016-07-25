#ifndef SYS_TYPES_H
#define SYS_TYPES_H

/* http://pubs.opengroup.org/onlinepubs/009696699/basedefs/sys/types.h.html */

/* Used for file sizes. */
#ifndef _OFF_T
#define _OFF_T
typedef signed int off_t;
#endif

/* Used for sizes of objects. */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

/* Used for a count of bytes or an error indication. */
#ifndef _SSIZE_T
#define _SSIZE_T
typedef signed int ssize_t;
#endif

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

#ifdef __KERNEL__
//FIXME: remove that header, and include <kernel/types.h> if just using u32, u16...
#include <kernel/types.h>
#endif /* __KERNEL__ */

#endif /* !SYS_TYPES_H */
