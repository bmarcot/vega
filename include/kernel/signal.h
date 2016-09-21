/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_SIGNAL_H
#define KERNEL_SIGNAL_H

#include <signal.h>

#include "linux/types.h"

struct ksignal {
	int ksig_signo;
	struct list_head ksig_list;
	struct sigaction ksig_struct;
};

#endif /* !KERNEL_SIGNAL_H */
