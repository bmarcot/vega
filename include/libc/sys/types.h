#ifndef SYS_TYPES_H
#define SYS_TYPES_H

/* http://pubs.opengroup.org/onlinepubs/009696699/basedefs/sys/types.h.html */

/* Used for clock ID type in the clock and timer functions. */
#ifndef _CLOCKID_T
#define _CLOCKID_T
typedef unsigned int clockid_t;
#endif

/* Used for file sizes. */
#ifndef _OFF_T
#define _OFF_T
typedef signed int off_t;
#endif

/* Used for process IDs and process group IDs. */
#ifndef _PID_T
#define _PID_T
typedef unsigned int pid_t;
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

/* Used for timer ID returned by timer_create(). */
#ifndef _TIMER_T
#define _TIMER_T
typedef unsigned int timer_t;
#endif

/* Used for user IDs. */
#ifndef _UID_T
#define _UID_T
typedef unsigned int uid_t;
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

typedef _Atomic int atomic_s32;

/* Used for mutexes. */
typedef struct mutex {
	atomic_s32 val;
} pthread_mutex_t;

/* Used to identify a mutex attribute object. */
typedef int pthread_mutexattr_t;

/* Used to identify a thread. */
typedef unsigned int pthread_t;

/* Used for condition variables. */
#include "linux/types.h"
typedef struct {
	struct list_head waitq;
} pthread_cond_t;

/* Used to identify a condition attribute object. */
 typedef int pthread_condattr_t;

#ifdef __KERNEL__
//FIXME: remove that header, and include <kernel/types.h> if just using u32, u16...
#include <kernel/types.h>
#endif /* __KERNEL__ */

#endif /* !SYS_TYPES_H */
