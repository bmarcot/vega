#ifndef SIGNAL_H
#define SIGNAL_H

#include <sys/types.h>

typedef struct {
	void *ss_sp;     /* Base address of stack */
	int ss_flags;    /* Flags */
	size_t ss_size;	 /* Number of bytes in stack */
} stack_t;

/* sigevent - structure for notification from asynchronous routines */

union sigval {            /* Data passed with notification */
	int sival_int;    /* Integer value */
	void *sival_ptr;  /* Pointer value */
};

struct sigevent {
	int sigev_notify; /* Notification method */
	int sigev_signo;  /* Notification signal */

	/* Data passed with notification */
	union sigval sigev_value;

	/* Function used for thread notification (SIGEV_THREAD) */
	void (*sigev_notify_function)(union sigval);

	/* Attributes for notification thread (SIGEV_THREAD) */
	void *sigev_notify_attributes;

	/* ID of thread to signal (SIGEV_THREAD_ID) */
	pid_t sigev_notify_thread_id;
};

#endif /* !SIGNAL_H */
