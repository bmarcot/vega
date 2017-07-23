/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

/* #include <signal.h> */

/* #include <vega/signal.h> */

/* #include <kernel/thread.h> */
/* #include <kernel/types.h> */

#include <kernel/types.h>
#include <uapi/kernel/signal.h>

struct ksignal {
	int              sig;
	struct sigaction sa;
	struct list_head list;
};

struct thread_info; //XXX: Will die, replace with task_struct.

void do_sigevent(struct thread_info *thread, /* struct task_struct *task, */
		struct sigevent *sigevent);

#endif /* !_KERNEL_SIGNAL_H */
