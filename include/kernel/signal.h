/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

#include <signal.h>

#include <kernel/thread.h>

#include "linux/types.h"

#define SIGMAX  31

struct signal_info {
	int              signo;
	struct list_head list;
	struct sigaction act_storage;
};

void do_sigevent(const struct sigevent *sigevent,
		struct thread_info *thread);

#endif /* !_KERNEL_SIGNAL_H */
