#ifndef LIBC_SIGNAL_H
#define LIBC_SIGNAL_H

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

/* sigaction - used to change the action taken by a process on receipt of a
     specific signal  */

typedef struct {
	int si_signo;
	/* int si_code; */
	union sigval si_value;
	/* int si_errno; */
	/* pid_t si_pid; */
	/* uid_t si_uid; */
	/* void *si_addr; */
	/* int si_status; */
	/* int si_band; */
} siginfo_t;

typedef int sigset_t;

struct sigaction {
	void (*sa_handler)(int);  /* Pointer to a signal-catching function
				     or one of the SIG_IGN or SIG_DFL.  */

	sigset_t sa_mask;  /* Set of signals to be blocked during execution
			      of the signal handling function.  */

	int sa_flags;  /* Special flags.  */

	/* Pointer to a signal-catching function.  */
	void (*sa_sigaction)(int, siginfo_t *, void *);
};

#define SA_SIGINFO  (1 << 0)

/* #define SIGKILL  9   /\* Kill (can't be caught or ignored) (POSIX) *\/ */
/* #define SIGUSR1  10  /\* User defined signal 1 (POSIX) *\/ */
/* #define SIGUSR2  12  /\* User defined signal 2 (POSIX) *\/ */
/* #define SIGSTOP  19  /\* Stop executing(can't be caught or ignored) (POSIX) *\/ */
#define SIGUSR1  0
#define SIGUSR2  1

int sigaction(int sig, const struct sigaction *restrict act,
	struct sigaction *restrict oact);
int raise(int sig);
int sigqueue(pid_t pid, int sig, const union sigval value);

#endif /* !LIBC_SIGNAL_H */
