/*
 * kernel/exit.c
 *
 * Copyright (c) 2017-2018 Ben Marcot
 */

#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

static int do_notify_parent(struct task_struct *tsk, int sig)
{
	/* task is not the task leader - nobody will wait for it */
	if (!thread_group_leader(tsk))
		return 1;

	/* This is valid because we only support vfork(), and parent is not
	 * running while child is running. */
	sched_enqueue(tsk->parent); // ???  wake_up_process...

	struct sighand_struct *sighand = tsk->parent->sighand;

	if (sighand && (sighand->action[SIGCHLD].sa_handler != 0/* SIG_IGN */)) {
		struct sigqueue q;
		q.info.si_signo = sig;
		q.info.si_code = 0 /* CLD_EXITED */;
		q.info._sigchld.si_pid = current->pid;
		q.info._sigchld.si_status = tsk->exit_code & 0x7f;
		send_signal_info(sig, &q, tsk->parent);
	}

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
