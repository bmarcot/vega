/*
 * include/uapi/kernel/signal.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _UAPI_KERNEL_SIGNAL_H
#define _UAPI_KERNEL_SIGNAL_H

#include <uapi/kernel/posix_types.h>

#define SIGKILL  9
#define SIGUSR1 10
#define SIGUSR2 12
#define SIGSTOP 19

#define SA_SIGINFO  0x1
#define SA_RESTORER 0x2

union sigval {
	int  sival_int;
	void *sival_ptr;
};

#define _NSIG 32

typedef struct {
	unsigned long sig[_NSIG / 32];
} sigset_t;

typedef struct {
	int          si_signo;
	union sigval si_value;
	pid_t        si_pid;
} siginfo_t;

struct sigaction {
	union {
		void  (*sa_handler)(int);
		void  (*sa_sigaction)(int, siginfo_t *, void *);
	};
	sigset_t      sa_mask;
	unsigned long sa_flags;
	void          (*sa_restorer)(void);
};

#define SIGEV_NONE   0
#define SIGEV_SIGNAL 1

struct sigevent {
	int          sigev_notify;
	int          sigev_signo;
	union sigval sigev_value;
};

#endif /* !_UAPI_KERNEL_SIGNAL_H */
