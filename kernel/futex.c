/*
 * kernel/futex.c
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#include <kernel/errno-base.h>
#include <kernel/futex.h>
#include <kernel/list.h>
#include <kernel/sched.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

#include <uapi/kernel/futex.h>

static LIST_HEAD(futexes);

int futex_wait(int *uaddr, int val)
{
	if (*uaddr == val) {
		struct futex futex = {
			.lock_ptr = uaddr,
			.task = current,
		};
		list_add(&futex.list, &futexes);

		set_current_state(TASK_UNINTERRUPTIBLE);

		schedule();
	}

	return 0;
}

int futex_wake(int *uaddr, int val)
{
	struct futex *futex, *temp;
	int wake_count = 0;

	if (!val)
		return 0;

	list_for_each_entry_safe(futex, temp, &futexes, list) {
		if (futex->lock_ptr == uaddr) {
			list_del(&futex->list);
			set_task_state(futex->task, TASK_RUNNING);
			wake_count++;
			if (--val == 0)
				break;
		}
	}

	return wake_count;
}

SYSCALL_DEFINE(futex,
	int		*uaddr,
	int		futex_op,
	int		val)
{
	if (futex_op == FUTEX_WAIT)
		return futex_wait(uaddr, val);
	else if (futex_op == FUTEX_WAKE)
		return futex_wake(uaddr, val);

	return -EINVAL; //-ENOSYS;
}
