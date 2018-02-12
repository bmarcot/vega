/*
 * include/kernel/sched/signal.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_SCHED_SIGNAL_H
#define _KERNEL_SCHED_SIGNAL_H

#include <kernel/types.h>

struct signal_struct {
	struct list_head	thread_head;
	int			group_exit_code;
	unsigned int		flags;
};

#define SIGNAL_GROUP_EXIT	0x00000004 /* group exit in progress */

static inline int signal_group_exit(const struct signal_struct *sig)
{
	return sig->flags & SIGNAL_GROUP_EXIT;
}

#endif /* !_KERNEL_SCHED_SIGNAL_H */
