/*
 * include/kernel/futex.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _KERNEL_FUTEX_H
#define _KERNEL_FUTEX_H

#include <kernel/types.h>

struct task_struct;

struct futex {
	int			*lock_ptr;
	struct list_head	wq_head;
	struct list_head	list;
};

int futex_wait(int *uaddr, int val);
int futex_wake(int *uaddr, int val);

#endif /* !_KERNEL_FUTEX_H */
