/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

#include <signal.h>

#include <vega/signal.h>

#include <kernel/thread.h>
#include <kernel/types.h>

#define SIGMAX  31

struct signal_info {
	int              signo;
	struct list_head list;
	struct sigaction act_storage;
};

/* new signals */

#include <kernel/list.h>
#include <uapi/kernel/signal.h>

struct ksignal {
	int              sig;
	struct sigaction sa;
	struct list_head list;
};

void do_sigevent(struct thread_info *thread, /* struct task_struct *task, */
		struct sigevent *sigevent);

#endif /* !_KERNEL_SIGNAL_H */
