/*
 * kernel/fork.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <string.h>

#include <kernel/kernel.h>
#include <kernel/mm.h>
#include <kernel/mm/page.h>
#include <kernel/resource.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/stddef.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

static struct task_struct *copy_process(int flags)
{
	struct task_struct *tsk;

	tsk = alloc_pages(size_to_page_order(THREAD_SIZE));
	if (!tsk)
		return NULL;
	init_task(tsk, flags);
	init_thread(tsk);

	return tsk;
}

SYSCALL_DEFINE(vfork, void)
{
	struct task_struct *child;
	//FIXME: Alloc saved context on stack; if stack too small, then use malloc
	struct cpu_user_context *saved_ctx;
	u32 saved_sp;

	child = copy_process(CLONE_VFORK);
	if (!child) {
		pr_err("Cannot create child process");
		return -1;
	}
	set_current_state(TASK_STOPPED);
	sched_enqueue(child);

	/* save current process context */
	saved_ctx = kmalloc(sizeof(*saved_ctx));
	if (!saved_ctx) {
		pr_err("Cannot save cpu context");
		return -1;
	}
	memcpy(saved_ctx, current->thread_info.user.ctx, sizeof(*saved_ctx));
	saved_sp = current->thread_info.user.psp;

	/* child thread points to parent's process */
	child->thread_info.user.psp = saved_sp;
	child->thread_info.user.ctx->r0 = 0;

	schedule();

	/* restore current process context */
	current->thread_info.user.psp = saved_sp;
	memcpy(current->thread_info.user.ctx, saved_ctx, sizeof(*saved_ctx));
	kfree(saved_ctx);

	return child->tgid;
}

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

	return tsk->pid;
}
