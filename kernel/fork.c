/*
 * kernel/fork.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <string.h>

#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/resource.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/stddef.h>
#include <kernel/syscalls.h>

#include <asm/current.h>
#include <asm/ptrace.h>

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

#define __process_alloca_with_align(__tsk, __ptr, __align) ({	\
	struct thread_info *__thrd = task_thread_info(__tsk);	\
	u32 __sp = __thrd->user.psp;				\
	__sp -= sizeof(__typeof__(*(__ptr)));			\
	__sp = align(__sp, __align);				\
	__thrd->user.psp = __sp;				\
	__ptr = (__typeof__(__ptr))__sp; })

#define __process_alloca(__tsk, __ptr) __process_alloca_with_align(__tsk, __ptr, 1)

SYSCALL_DEFINE(vfork, void)
{
	struct task_struct *child;
	struct pt_regs *child_ctx;

	child = copy_process(CLONE_VFORK);
	if (!child) {
		pr_err("Cannot create child process");
		return -1;
	}

	/* child thread uses its parent's process stack */
	child->thread_info.user.psp = current->thread_info.user.psp;

	/* after taking an exception, stack is aligned on 8-bytes */
	__process_alloca(child, child_ctx);

	/* copy parent's user frame, but update the return value */
	memcpy(child_ctx, current->thread_info.user.regs, sizeof(*child_ctx));
	child_ctx->r0 = 0;

	/* parent is now stopped until child returns */
	sched_dequeue(current);
	set_current_state(TASK_UNINTERRUPTIBLE);
	sched_enqueue(child);

	schedule();

	return child->tgid;
}

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
	sched_enqueue(tsk);

	return tsk->pid;
}
