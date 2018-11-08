/*
 * kernel/fork.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/stddef.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>

#include <asm/current.h>
#include <asm/ptrace.h>

struct task_struct *
do_clone(unsigned long flags, void *child_stack, struct pt_regs *regs)
{
	struct task_struct *tsk;

	tsk = alloc_pages(size_to_page_order(THREAD_SIZE));
	if (!tsk)
		return NULL;
	init_task(tsk, flags);
	arch_thread_setup(tsk, flags, child_stack, regs);

	return tsk;
}

SYSCALL_DEFINE(vfork, void)
{
	struct task_struct *child;

	/* keep the same process stack, duplicate current's process registers */
	child = do_clone(0, (void *)current->thread_info.user.psp,
			current->thread_info.user.regs);

	/* update return value */
	child->thread_info.user.regs->r0 = 0; // arch_thread_set_retval(child, 0);

	/* calling thread is suspended until the child terminates */
	set_task_state(current, TASK_UNINTERRUPTIBLE);
	set_task_state(child, TASK_RUNNING);

	schedule();

	return child->tgid;
}

SYSCALL_DEFINE(clone,
	unsigned long	flags,
	void		*child_stack,
	struct pt_regs	*regs)
{
	struct task_struct *tsk;

	/* flags must also include CLONE_SIGHAND if CLONE_THREAD is specified */
	if ((flags & CLONE_THREAD) && !(flags & CLONE_SIGHAND))
		return -1;

	tsk = do_clone(flags, child_stack, regs);
	if (!tsk)
		return -1;
	set_task_state(tsk, TASK_RUNNING);

	return tsk->pid;
}
