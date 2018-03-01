/*
 * kernel/signal.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <errno.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>
#include <kernel/thread_info.h>

#include <asm/current.h>

int signal_pending(struct task_struct *tsk)
{
	return !sigisemptyset(&tsk->pending.signal);
}

static struct sigaction *task_sigaction(struct task_struct *tsk, int sig)
{
	return &tsk->sighand->action[sig];
}

struct signal_struct *alloc_signal_struct(struct task_struct *tsk)
{
	struct signal_struct *signal;

	signal = kzalloc(sizeof(*signal));
	if (!signal)
		return NULL;
	INIT_LIST_HEAD(&signal->thread_head);

	return signal;
}

struct sighand_struct *copy_sighand_struct(struct task_struct *tsk)
{
	struct sighand_struct *sighand;

	sighand = kmalloc(sizeof(*sighand));
	if (!sighand)
		return NULL;
	memcpy(sighand, tsk->sighand, sizeof(*sighand));

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

	/* save and install a new handler */
	sighand = task_sighand(current);
	k_act = &sighand->action[signum];
	if (oldact)
		memcpy(oldact, k_act, sizeof(*k_act));
	memcpy(k_act, act, sizeof(*k_act));

	return 0;
}

int send_signal_info(int sig, struct sigqueue *info, struct task_struct *tsk)
{
	if (info)
		list_add_tail(&info->list, &tsk->pending.list);
	sigaddset(&tsk->pending.signal, sig);

	set_ti_thread_flag(task_thread_info(tsk), TIF_SIGPENDING);

	if (tsk->state != TASK_RUNNING)
		return wake_up_process(tsk);

	return 0;
}

int send_rt_signal(struct task_struct *tsk, int sig, int value)
{
	struct sigqueue *q;

	q = kmalloc(sizeof(*q));
	if (!q)
		return -1;
	q->flags = 0;
	q->info.si_signo = sig;
	q->info._rt.si_pid = current->pid;
	q->info._rt.si_value.sival_int = value;
	send_signal_info(sig, q, tsk);

	return 0;
}

void do_signal(void)
{
	//FIXME: Revisit handling of signals that are barely a bit in the set
	if (sigismember(&current->pending.signal, SIGKILL))
		do_exit(SIGKILL);

	struct sigqueue *sig = list_first_entry(&current->pending.list,
						struct sigqueue, list);
	int signo = sig->info.si_signo;
	sigdelset(&current->pending.signal, signo);
	__do_signal(signo, sig /* or NULL */);
}

void zap_all_threads(struct task_struct *tsk);

static int do_kill(int pid, int sig, int value)
{
	struct task_struct *tsk;

	tsk = get_task_by_pid(pid);
	if (!tsk) {
		//errno = ESRCH;
		return -1;
	}

	/* process SIGKILL early */
	if (sig == SIGKILL) {
		tsk->signal->group_exit_code = SIGKILL;
		tsk->signal->flags = SIGNAL_GROUP_EXIT;
		zap_all_threads(tsk);
		return 0;
	}

	/* it's ok to have no handlers installed */
	if (!tsk->sighand)
		return 0;
	if (!tsk->sighand->action[sig].sa_handler)
		return 0;

	send_rt_signal(tsk, sig, value);

	return 0;
}

SYSCALL_DEFINE(sigqueue,
	pid_t		pid,
	int		sig,
	union sigval	value)
{
	return do_kill(pid, sig, value.sival_int);
}

SYSCALL_DEFINE(kill,
	pid_t		pid,
	int		sig)
{
	return do_kill(pid, sig, 0);
}

SYSCALL_DEFINE(tgkill,
	int		tgid,
	int		tid,
	int		sig)
{
	/* a thread-id (tid) is system-wide unique */
	return do_kill(tid, sig, 0);
}

SYSCALL_DEFINE(sigreturn, void)
{
	struct sigqueue *sig;
	int off;

	//FIXME: Check if we are actually returning from a signal handler

	sig = list_first_entry(&current->pending.list, struct sigqueue, list);
	off = sizeof(struct cpu_user_context);
	struct sigaction *act = task_sigaction(current, sig->info.si_signo);
	if (act->sa_flags & SA_SIGINFO)
		off += align_next(sizeof(siginfo_t), 8);
	current_thread_info()->user.psp += off;

	list_del(&sig->list);
	if (!(sig->flags & SIGQUEUE_PREALLOC))
		kfree(sig);

	/* If the interrupted task was in a syscall, this restores the
	 * syscall's return value. If it was interrupted because of an
	 * external interrupt, this restores the r0 value at the time of
	 * the interrupt. */
	return current_thread_info()->user.ctx->r0;
}

SYSCALL_DEFINE(pause, void)
{
	sched_dequeue(current);
	set_current_state(TASK_INTERRUPTIBLE);

	schedule();

	//FIXME: Only signals can wake-up this task

	return -EINTR;
}
