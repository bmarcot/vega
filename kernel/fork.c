/*
 * kernel/fork.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/mm/page.h>
#include <kernel/resource.h>
#include <kernel/sched.h>
#include <kernel/stddef.h>
#include <kernel/syscalls.h>

#include <asm/current.h>
#include <asm/thread_info.h>

#include <kernel/kernel.h>

SYSCALL_DEFINE(vfork, void)
{
	struct task_struct *child;
	char *child_stack;
	struct rlimit stacklim;
	unsigned int stackorder;

	/* get process stack size */
	do_getrlimit(RLIMIT_STACK, &stacklim);
	stackorder = size_to_page_order(stacklim.rlim_cur);

	/* allocate process stack */
	child_stack = alloc_pages(stackorder);
	if (!child_stack) {
		pr_err("Cannot allocate child stack");
		return -1;
	}

	/* Clone the current task, entry of the new task points to the return
	 * instruction of the syscall. */
	child = clone_task((void *)current->thread_info.user.ctx->ret_addr,
			child_stack + 504, CLONE_VFORK, NULL);
	if (!child) {
		pr_err("Cannot create child process");
		free_pages((unsigned long)child_stack, stackorder);
		return -1;
	}
	child->thread_info.user.ctx->lr = current->thread_info.user.ctx->lr;
	child->parent = current;

	set_current_state(TASK_STOPPED);
	sched_enqueue(child);

	schedule();

	return child->tgid;
}

#define THREAD_SIZE 512

struct task_struct *clone_task(int (*fn)(void *), void *child_stack,
			int flags, void *arg)
{
	struct task_struct *tsk;

	tsk = alloc_pages(size_to_page_order(THREAD_SIZE));
	if (!tsk)
		return NULL;
	init_task(tsk, flags);
	arch_thread_setup(tsk, fn, arg, child_stack);

	return tsk;
}

SYSCALL_DEFINE(clone,
	int		(*fn)(void *),
	void		*child_stack,
	int		flags,
	void		*arg)
{
	struct task_struct *tsk;

	tsk = clone_task(fn, child_stack, flags, arg);
	if (!tsk)
		return -1;
	sched_enqueue(tsk);

	return tsk->tid;
}
