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

void do_sigevent(struct task_struct *task, struct sigevent *sigevent);

#endif /* !_KERNEL_SIGNAL_H */
