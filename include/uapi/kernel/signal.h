/*
 * include/uapi/kernel/signal.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 *
 */

#ifndef _UAPI_KERNEL_SIGNAL_H
#define _UAPI_KERNEL_SIGNAL_H

#include <asm/bitsperlong.h>
#include <asm/posix_types.h>

#define _NSIG		32
#define _NSIG_BPW	__BITS_PER_LONG
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

#define SIGABRT		1	/* Core - Process abort signal. */
#define SIGALRM		2	/* Term - Alarm clock. */
#define SIGBUS		3	/* Core - Access to an undefined portion of a memory object. */
#define SIGCHLD		4	/* Ign  - Child process terminated, stopped, or continued. */
#define SIGCONT		5	/* Cont - Continue executing, if stopped. */
#define SIGFPE		6	/* Core - Erroneous arithmetic operation. */
#define SIGHUP		7	/* Term - Hangup. */
#define SIGILL		8	/* Core - Illegal instruction. */
#define SIGINT		9	/* Term - Terminal interrupt signal. */
#define SIGKILL		10	/* Term - Kill (cannot be caught or ignored). */
#define SIGPIPE		11	/* Term - Write on a pipe with no one to read it. */
#define SIGQUIT		12	/* Core - Terminal quit signal. */
#define SIGSEGV		13	/* Core - Invalid memory reference. */
#define SIGSTOP		14	/* Stop - Stop executing (cannot be caught or ignored). */
#define SIGTERM		15	/* Term - Termination signal. */
#define SIGTSTP		16	/* Stop - Terminal stop signal. */
#define SIGTTIN		17	/* Stop - Background process attempting read. */
#define SIGTTOU		18	/* Stop - Background process attempting write. */
#define SIGUSR1		19	/* Term - User-defined signal 1. */
#define SIGUSR2		20	/* Term - User-defined signal 2. */
#define SIGPOLL		21	/* Term - Pollable event. */
#define SIGPROF		22	/* Term - Profiling timer expired. */
#define SIGSYS		23	/* Core - Bad system call. */
#define SIGTRAP		24	/* Core - Trace/breakpoint trap. */
#define SIGURG		25	/* Ign  - High bandwidth data is available at a socket. */
#define SIGVTALRM	26	/* Term - Virtual timer expired. */
#define SIGXCPU		27	/* Core - CPU time limit exceeded. */
#define SIGXFSZ		28	/* Core - File size limit exceeded. */

#define SIGRTMIN	29
#define SIGRTMAX	_NSIG

#define SA_SIGINFO	0x1
#define SA_RESTORER	0x2

#define SIG_BLOCK	1	/* for blocking signals */
#define SIG_UNBLOCK	2	/* for unblocking signals */
#define SIG_SETMASK	3	/* for setting the signal mask */

typedef union sigval {
	int		sival_int;
	void		*sival_ptr;
} sigval_t;

typedef struct {
	unsigned long	sig[_NSIG_WORDS];
} sigset_t;

typedef struct siginfo {
	int	si_signo;
	int	si_errno;
	int	si_code;

	union {
		/* SIGCHLD */
		struct {
			pid_t	si_pid;
			int	si_status;
		} _sigchld;

		/* POSIX.1b timers */
		struct {
			sigval_t si_value;
		} _timer;

		/* POSIX.1b signals, sigqueue() */
		struct {
			pid_t	si_pid;
			sigval_t si_value;
		} _rt;
	};
} siginfo_t;

struct sigaction {
	union {
		void	(*sa_handler)(int);
		void	(*sa_sigaction)(int, siginfo_t *, void *);
	};
	sigset_t	sa_mask;
	unsigned long	sa_flags;
	void		(*sa_restorer)(void);
};

#define SIGEV_NONE	0
#define SIGEV_SIGNAL	1

struct sigevent {
	int		sigev_notify;
	int		sigev_signo;
	union sigval	sigev_value;
};

/*
 * SIGCHLD si_codes
 */
#define CLD_EXITED	1	/* child has exited */
#define CLD_KILLED	2	/* child was killed */
#define CLD_DUMPED	3	/* child terminated abnormally */
#define CLD_TRAPPED	4	/* traced child has trapped */
#define CLD_STOPPED	5	/* child has stopped */
#define CLD_CONTINUED	6	/* stopped child has continued */
#define NSIGCHLD	6

#endif /* !_UAPI_KERNEL_SIGNAL_H */
