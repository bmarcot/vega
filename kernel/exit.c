/*
 * kernel/exit.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

static void exit_notify(struct task_struct *tsk)
{
	int autoreap;

	if (thread_group_leader(tsk)) {
		//autoreap = do_notify_parent(tsk, tsk->exit_signal); // send_signal...
		autoreap = 0;
	} else {
		autoreap = 1;
	}

	tsk->state = autoreap ? EXIT_DEAD : EXIT_ZOMBIE;

	/* If the process is dead, release it - nobody will wait for it */
	if (autoreap)
		release_task(tsk);
}

static void do_exit(int status)
{
	struct task_struct *tsk = current;

	if (tsk->flags & CLONE_VFORK)
		sched_enqueue(tsk->parent); // send signal? do_notify_parent()...

	// mm_release();
	tsk->exit_code = status;
	exit_notify(tsk);

	if (tsk->state == EXIT_DEAD)
		tsk->state = TASK_DEAD;

	schedule();
}

SYSCALL_DEFINE(exit, int status)
{
	do_exit(status);

	return 0; /* never reached */
}

SYSCALL_DEFINE(exit_group, int status)
{
	if (!thread_group_leader(current)) {
		pr_err("Unsupported for other threads than leader");
		for (;;)
			;
	}

	do_exit(status);

	return 0; /* never reached */
}

SYSCALL_DEFINE(waitpid,
	pid_t		pid,
	int		*status,
	int		options)
{
	struct task_struct *tsk;

	tsk = get_task_by_pid(pid);
	if (!tsk)
		return -1;
	if (status)
		*status = tsk->exit_code; //FIXME: Check user pointer
	release_task(tsk);
	put_task_struct(tsk);

	return pid;
}
