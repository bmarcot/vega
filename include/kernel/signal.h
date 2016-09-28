/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_SIGNAL_H
#define KERNEL_SIGNAL_H

#include <signal.h>

#include "linux/types.h"

#define ERR_SIGNAL_UNSUPPORTED  -1
#define ERR_SIGNAL_UNHANDLED    -2

#define SIGMAX  31

struct ksignal {
	int ksig_signo;
	struct list_head ksig_list;
	struct sigaction ksig_struct;
};

#endif /* !KERNEL_SIGNAL_H */
