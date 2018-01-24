/*
 * include/uapi/kernel/signal.h
 *
 * Copyright (c) 2017-2018 Ben Marcot
 *
 */

#ifndef _UAPI_KERNEL_SIGNAL_H
#define _UAPI_KERNEL_SIGNAL_H

#include <asm/bitsperlong.h>
#include <asm/posix_types.h>

#define SIGHUP		1
#define SIGINT		2
#define SIGQUIT		3
#define SIGILL		4
#define SIGTRAP		5
#define SIGABRT		6
#define SIGIOT		6
#define SIGBUS		7
#define SIGFPE		8
#define SIGKILL		9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO

#define SA_SIGINFO	0x1
#define SA_RESTORER	0x2

typedef union sigval {
	int		sival_int;
	void		*sival_ptr;
} sigval_t;

#define _NSIG		32
#define _NSIG_BPW	__BITS_PER_LONG
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

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

#endif /* !_UAPI_KERNEL_SIGNAL_H */
