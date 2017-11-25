/*
 * kernel/signal.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <errno.h>
#include <string.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>
#include <kernel/thread_info.h>

#include <asm/current.h>

int signal_pending(struct task_struct *tsk)
{
	return tsk->pending.signal != 0;
}

//FIXME: Rename to task_sighand()
static struct sighand_struct *task_sighand_struct(struct task_struct *tsk)
{
	if (tsk->sighand)
		return tsk->sighand;

	return tsk->group_leader->sighand;
}

static struct sighand_struct *set_alloc_sighand_struct(struct task_struct *tsk)
{
	struct sighand_struct *sighand;

	sighand = kzalloc(sizeof(*sighand));
	if (!sighand)
		return NULL;
	current->sighand = sighand;
	current->group_leader->sighand = sighand;

	return sighand;
}

SYSCALL_DEFINE(sigaction,
	int			signum,
	const struct sigaction	*act,
	struct sigaction	*oldact)
{
	struct sighand_struct *sighand;
	struct sigaction *k_act;

	if ((signum == SIGKILL) || (signum == SIGSTOP)) {
		errno = EINVAL;
		return -1;
	}

	/* do checks on act variable: is address valid, not nil, etc. */
	if (!act) {
		errno = EFAULT;
		return -1;
	}

	/* alloc or get the signal handler table */
	sighand = task_sighand_struct(current);
	if (!sighand) {
		sighand = set_alloc_sighand_struct(current);
		if (!sighand)
			return -1;
	}

	/* save and install a new handler */
	k_act = &sighand->action[signum];
	if (oldact)
		memcpy(oldact, k_act, sizeof(*k_act));
	memcpy(k_act, act, sizeof(*k_act));

	return 0;
}

int notify_signal(struct task_struct *tsk, int signo, int value)
{
	struct sigqueue *sig;

	sig = kmalloc(sizeof(*sig));
	if (!sig)
		return -1;
	sig->info.si_signo = signo;
	sig->info.si_value.sival_int = value;
	sig->info.si_pid = current->pid;
	list_add_tail(&sig->list, &tsk->pending.list);
	tsk->pending.signal |= (1 << signo);

	set_ti_thread_flag(task_thread_info(tsk), TIF_SIGPENDING);
	if (tsk->state != TASK_RUNNING)
		return wake_up_process(tsk);

	return 0;
}

static int do_kill(int pid, int sig, union sigval value)
{
	struct task_struct *tsk;

	tsk = get_task_by_pid(pid);
	if (!tsk) {
		//errno = ESRCH;
		return -1;
	}

	if (sig == SIGKILL)
		notify_signal(tsk, sig, 0);

	/* it's ok to have no handlers installed */
	if (!tsk->sighand)
		return 0;
	if (!tsk->sighand->action[sig].sa_handler)
		return 0;

	notify_signal(tsk, sig, value.sival_int);

	return 0;
}

SYSCALL_DEFINE(sigqueue,
	pid_t		pid,
	int		sig,
	union sigval	value)
{
	return do_kill(pid, sig, value);
}

SYSCALL_DEFINE(kill,
	pid_t		pid,
	int		sig)
{
	return do_kill(pid, sig, (union sigval){0});
}

SYSCALL_DEFINE(tgkill,
	int		tgid,
	int		tid,
	int		sig)
{
	/* a TID is system-wide unique */
	return do_kill(tid, sig, (union sigval){0});
}

SYSCALL_DEFINE(sigreturn, void)
{
	struct sigaction *act;
	int off;

	if (!signal_pending(current)) {
		pr_warn("Wants to return from signal, but no signal raised");
		return 0;

	}

	struct sigqueue *sig = list_first_entry(&current->pending.list,
						struct sigqueue, list);
	act = &current->sighand->action[sig->info.si_signo];
	off = sizeof(struct cpu_user_context);
	if (act->sa_flags & SA_SIGINFO) // not act, sig->flags instead
		off += align_next(sizeof(siginfo_t), 8);
	current_thread_info()->user.psp += off;

	list_del(&sig->list);
	sig->info.si_signo = 0;//clear_bit(sig->info.si_signo);
	kfree(sig);

	/* If the interrupted task was in a syscall, this restores the
	 * syscall's return value. If it was interrupted because of an
	 * external interrupt, this restores the r0 value at the time of
	 * the interrupt. */
	return current_thread_info()->user.ctx->r0;
}

void signal_event(struct task_struct *tsk, struct sigevent *sigev)
{
	int value = sigev->sigev_value.sival_int;

	if (sigev->sigev_notify != SIGEV_SIGNAL)
		return;
	notify_signal(tsk, sigev->sigev_signo, value);
}

SYSCALL_DEFINE(pause, void)
{
	sched_dequeue(current);
	set_current_state(TASK_INTERRUPTIBLE);

	schedule();

	//FIXME: Only signals can wake-up this task

	return -EINTR;
}
