/*
 * kernel/fork.c
 *
 * Copyright (c) 2017 Benoit Marcot
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
	struct cpu_user_context *child_ctx;

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
	memcpy(child_ctx, current->thread_info.user.ctx, sizeof(*child_ctx));
	child_ctx->r0 = 0;

	/* parent is now stopped until child returns */
	sched_dequeue(current);
	set_current_state(TASK_STOPPED);
	sched_enqueue(child);

	schedule();

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
