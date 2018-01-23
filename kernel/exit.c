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

static int do_notify_parent(struct task_struct *tsk, int sig)
{
	if (!thread_group_leader(tsk))
		return -1;
	sched_enqueue(tsk->parent);
	notify_signal(tsk->parent, sig, 0);

	return 0;
}

static void exit_notify(struct task_struct *tsk)
{
	int autoreap;

	autoreap = do_notify_parent(tsk, tsk->exit_signal);
	tsk->state = autoreap ? EXIT_DEAD : EXIT_ZOMBIE;

	/* If the process is dead, release it - nobody will wait for it */
	if (autoreap)
		release_task(tsk);
}

void do_exit(int status)
{
	struct task_struct *tsk = current;

	sched_dequeue(tsk);
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
	if (tsk->state != EXIT_ZOMBIE)
		return -1;
	if (status)
		*status = tsk->exit_code; //FIXME: Check user pointer
	release_task(tsk);
	put_task_struct(tsk);

	return pid;
}
