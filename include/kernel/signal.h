/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

#include <kernel/types.h>
#include <uapi/kernel/signal.h>

struct ksignal {
	int              sig;
	struct sigaction sa;
	struct list_head list;
};

struct sighand_struct {
	struct sigaction action[_NSIG];
};

void signal_event(struct task_struct *tsk, struct sigevent *sigev);

#endif /* !_KERNEL_SIGNAL_H */
