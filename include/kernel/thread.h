/*
 * include/kernel/thread.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_THREAD_H
#define _KERNEL_THREAD_H

#include <sys/pthread_types.h>
#include <kernel/kernel.h>

#define INTR_STACK_ORDER  9  /* 512 Bytes */
#define INTR_STACK_SIZE   (1 << INTR_STACK_ORDER)

/* forward declarations */

struct thread_info;

#define CURRENT_THREAD_INFO(var) \
	struct thread_info *var = current_thread_info();

#define THREAD_INFO(addr)					\
	({ (struct thread_info *)align((unsigned long)addr,	\
					INTR_STACK_SIZE); })

#endif /* !_KERNEL_THREAD_H */
