/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

#include <kernel/list.h>
#include <uapi/kernel/signal.h>

struct sigqueue {
	int			flags;
	siginfo_t		info;
	struct list_head	list;
};

struct sigpending {
	/* sigset_t signal; */
	unsigned int		signal;
	struct list_head	list;
};

struct sighand_struct {
	struct sigaction	action[_NSIG];
};

struct task_struct;

int signal_pending(struct task_struct *tsk);
int send_signal(int sig, int value);
void signal_event(struct task_struct *tsk, struct sigevent *sigev);

static inline void init_sigpending(struct sigpending *sig)
{
	sig->signal = 0; //sigemptyset(&sig->signal);
	INIT_LIST_HEAD(&sig->list);
}

#endif /* !_KERNEL_SIGNAL_H */
