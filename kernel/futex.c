/*
 * kernel/futex.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/errno-base.h>
#include <kernel/futex.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/syscalls.h>
#include <kernel/wait.h>

#include <asm/current.h>

#include <uapi/kernel/futex.h>

static LIST_HEAD(futexes);

static struct futex *find_futex_by_lock_ptr(int *uaddr)
{
	struct futex *futex;

	list_for_each_entry(futex, &futexes, list) {
		if (futex->lock_ptr == uaddr)
			return futex;
	}

	return NULL;
}

int futex_wait(int *uaddr, int val)
{
	struct futex *futex;
	int retval;

	if (*uaddr != val)
		return 0;

	futex = find_futex_by_lock_ptr(uaddr);
	if (!futex) {
		futex = kmalloc(sizeof(struct futex));
		futex->lock_ptr = uaddr;
		INIT_LIST_HEAD(&futex->wq_head);
		list_add(&futex->list, &futexes);
	}
	retval = wait_event(&futex->wq_head, *uaddr != val);
	if (list_empty(&futex->wq_head)) {
		list_del(&futex->list);
		kfree(futex);
	}

	return retval;
}

int futex_wake(int *uaddr, int val)
{
	struct futex *futex;

	if (!val)
		return 0;
	futex = find_futex_by_lock_ptr(uaddr);

	return wake_up(&futex->wq_head, val);
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
