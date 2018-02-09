/*
 * include/kernel/sched/signal.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_SCHED_SIGNAL_H
#define _KERNEL_SCHED_SIGNAL_H

#include <kernel/signal_types.h>

struct signal_struct {
	struct sigpending	pending;
};

#endif /* !_KERNEL_SCHED_SIGNAL_H */
