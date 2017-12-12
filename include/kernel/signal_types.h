/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_TYPES_H
#define _KERNEL_SIGNAL_TYPES_H

#include <kernel/sched.h>
#include <kernel/types.h>

#include <uapi/kernel/signal.h>

struct sigqueue {
	siginfo_t		info;
	struct list_head	list;
};

struct sigpending {
	sigset_t		signal;
	struct list_head	list;
};

struct sighand_struct {
	struct sigaction	action[_NSIG];
};

#endif /* !_KERNEL_SIGNAL_TYPES_H */
