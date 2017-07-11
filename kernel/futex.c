/*
 * kernel/futex.c
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#include <kernel/kernel.h> //temp

#include <kernel/errno-base.h>
#include <kernel/futex.h>
#include <kernel/sched.h>

#include <asm/current.h>

#include <uapi/kernel/futex.h>

#include "linux/list.h"

static LIST_HEAD(futexes);

int futex_wait(int *uaddr, int val)
{
	if (*uaddr == val) {
		struct futex futex = {
			.lock_ptr = uaddr,
			.task = get_current(),
		};
		list_add(&futex.list, &futexes);
		/* pr_info("-- go to sleep with lockptr=%p", futex.lock_ptr); */
		schedule();
	}

	return 0;
}

int futex_wake(int *uaddr, int val)
{
	struct futex *futex;
	int wake_count = 0;

	if (!val)
		return 0;

	list_for_each_entry(futex, &futexes, list) {
		/* pr_info("TOTO %p uaddr=%p", futex->lock_ptr, uaddr); */
		if (futex->lock_ptr == uaddr) {
			/* pr_info("woke up a thread!"); */
			list_del(&futex->list);
			sched_enqueue(futex->task); // task_wake();
			wake_count++;
			if (--val == 0)
				break;
		}
	}

	return wake_count;
}

int sys_futex(int *uaddr, int futex_op, int val)
{
	pr_info("uaddr=%p, futex_op=%d, val=%d", uaddr, futex_op, val);

	if (futex_op == FUTEX_WAIT)
		return futex_wait(uaddr, val);
	else if (futex_op == FUTEX_WAKE)
		return futex_wake(uaddr, val);

	return -EINVAL; //-ENOSYS;
}
