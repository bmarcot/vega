/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

#include <signal.h>

#include "linux/types.h"

#define SIGMAX  31

struct signal_info {
	int signo;
	struct list_head list;
	struct sigaction act_storage;
};

#endif /* !_KERNEL_SIGNAL_H */
