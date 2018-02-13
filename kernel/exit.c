/*
 * kernel/exit.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/exit.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

void zap_all_threads(struct task_struct *tsk)
{
	struct task_struct *t, *n;

	list_for_each_entry_safe(t, n, &tsk->signal->thread_head, thread_group) {
		send_signal_info(SIGKILL, NULL, t);
	}
}

static int do_notify_parent(struct task_struct *tsk, int sig)
{
	struct sighand_struct *sighand;
	struct sigqueue q;

	/* task is not the task leader - nobody will wait for it */
	if (!thread_group_leader(tsk))
		return 1;

	/* This is valid because we only support vfork(), and parent is not
	 * running while child is running. */
	sched_enqueue(tsk->parent); // ???  wake_up_process...

	sighand = tsk->parent->sighand;
	if (sighand && (sighand->action[SIGCHLD].sa_handler != 0/* SIG_IGN */)) {
		q.flags = SIGQUEUE_PREALLOC;
		q.info.si_signo = sig;
		q.info.si_code = tsk->exit_code & EXIT_FATAL ? CLD_KILLED : CLD_EXITED;
		q.info._sigchld.si_pid = current->pid;
		q.info._sigchld.si_status = (tsk->exit_code >> 8) & 0x7f;
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

void do_exit(int exit_code)
{
	sched_dequeue(current);
	// mm_release();
	if (signal_group_exit(current->signal))
		current->exit_code = current->signal->group_exit_code;
	else
		current->exit_code = exit_code;
	exit_notify(current);
	if (current->state == EXIT_DEAD)
		current->state = TASK_DEAD;

	schedule();

	/* not reached */
}

SYSCALL_DEFINE(exit, int exit_code)
{
	do_exit(exit_code);

	/* not reached */
	return 0;
}

SYSCALL_DEFINE(exit_group, int exit_code)
{
	struct signal_struct *sig = current->signal;

	/* WEXITSTATUS are bits 15..8 and WTERMSIG are bits 7..0 */
	exit_code = (exit_code & 0x7f) << 8;
	sig->group_exit_code = exit_code;
	sig->flags = SIGNAL_GROUP_EXIT;

	/* Send SIGKILL to all threads in the current thread-group. */
	//FIXME: No need to send signal to current thread
	zap_all_threads(current);

	do_exit(exit_code);

	/* not reached */
	return 0;
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
