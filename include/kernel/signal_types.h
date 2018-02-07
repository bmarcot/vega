/*
 * include/kernel/signal_types.h
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_TYPES_H
#define _KERNEL_SIGNAL_TYPES_H

#include <kernel/sched.h>
#include <kernel/types.h>

#include <uapi/kernel/signal.h>

struct sigqueue {
	struct list_head	list;
	int			flags;
	siginfo_t		info;
};

#define SIGQUEUE_PREALLOC	1

struct sigpending {
	sigset_t		signal;
	struct list_head	list;
};

struct sighand_struct {
	struct sigaction	action[_NSIG];
};

#endif /* !_KERNEL_SIGNAL_TYPES_H */
