/*
 * include/uapi/kernel/signal.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _UAPI_KERNEL_SIGNAL_H
#define _UAPI_KERNEL_SIGNAL_H

#define SA_SIGINFO  0x1
#define SA_RESTORER 0x2

union sigval {
	int  sival_int;
	void *sival_ptr;
};

#define _NSIGS 32

typedef struct {
	unsigned long sig[_NSIGS / 32];
} sigset_t;

typedef struct {
	int si_signo;
	/* int si_code; */
	union sigval si_value;
	/* int si_errno; */
	pid_t si_pid;
	/* uid_t si_uid; */
	/* void *si_addr; */
	/* int si_status; */
	/* int si_band; */
} siginfo_t;

struct sigaction {
	union {
		void  (*sa_handler)(int);
		void  (*sa_sigaction)(int, siginfo_t *, void *);
	};
	sigset_t      sa_mask;
	unsigned long sa_flags;
	void          (*sa_restorer)(void);

	//XXX: Will die.
	struct list_head sa_list;
	int sa_signo;
};

#endif /* !_UAPI_KERNEL_SIGNAL_H */
