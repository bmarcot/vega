/*
 * kernel/fork.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/stddef.h>

#include <asm/current.h>
#include <asm/thread_info.h>

int do_fork(void)
{
	/* create a new child process */
	char *child_stack = alloc_pages(size_to_page_order(512));

	struct task_struct *child = clone_task(current->thread_info.user.ctx->ret_addr,
			child_stack + 504, 0, NULL);
	if (child == NULL)
		return -1;
	// child->parent = get_current();
	child->thread_info.user.ctx->lr =
		current->thread_info.user.ctx->lr | 1;
	sched_enqueue(child);

	return child->pid;
}

int sys_fork(void)
{
	return do_fork();
}

#define THREAD_SIZE 512

struct task_struct *clone_task(int (*fn)(void *), void *child_stack,
			int flags, void *arg)
{
	struct task_struct *tsk;

	tsk = alloc_pages(size_to_page_order(THREAD_SIZE));
	if (!tsk)
		return NULL;
	init_task(tsk);
	tsk->flags = flags;
	arch_thread_setup(tsk, fn, arg, child_stack);

	return tsk;
}

int sys_clone(int (*fn)(void *), void *child_stack, int flags, void *arg)
{
	struct task_struct *tsk;

	tsk = clone_task(fn, child_stack, flags, arg);
	if (!tsk)
		return -1;
	sched_enqueue(tsk);

	return tsk->pid;
}
