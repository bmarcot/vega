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
#include <kernel/mm/page.h>
#include <kernel/mm/slab.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>
#include <kernel/thread_info.h>

#include <asm/current.h>
#include <asm/ptrace.h>

static struct kmem_cache *signal_struct_cache;
static struct kmem_cache *sigqueue_cache;

int signal_pending(struct task_struct *tsk)
{
	return !sigisemptyset(&tsk->pending.signal);
}

static struct sigaction *task_sigaction(struct task_struct *tsk, int sig)
{
	return &tsk->sighand->action[sig - 1];
}

struct signal_struct *alloc_signal_struct(void)
{
	struct signal_struct *sig;

	sig = kmem_cache_alloc(signal_struct_cache, CACHE_OPT_NONE);
	if (!sig)
		return NULL;
	memset(sig, 0, sizeof(*sig));
	INIT_LIST_HEAD(&sig->thread_head);

	return sig;
}

void put_signal_struct(struct signal_struct *sig)
{
	kmem_cache_free(signal_struct_cache, sig);
}

struct sighand_struct *copy_sighand_struct(struct task_struct *tsk)
{
	struct sighand_struct *sig;

	/* sighand_struct size must be a power of 2, so we allocate pages */
	_Static_assert((sizeof(struct sighand_struct) &
				(sizeof(struct sighand_struct) - 1)) == 0,
		"sighand_struct size must be a power of 2");

	sig = alloc_pages(size_to_page_order(sizeof(*sig)));
	if (!sig)
		return NULL;
	memcpy(sig, tsk->sighand, sizeof(*sig));

	return sig;
}

void put_sighand_struct(struct sighand_struct *sig)
{
	free_pages((unsigned long)sig, size_to_page_order(sizeof(*sig)));
}

SYSCALL_DEFINE(sigaction,
	int			signum,
	const struct sigaction	*act,
	struct sigaction	*oldact)
{
	struct sighand_struct *sighand;
	struct sigaction *k_act;

	if (sig_kernel_only(signum)) {
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
	k_act = &sighand->action[signum - 1];
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

	if (!sigismember(&tsk->blocked, sig)) {
		set_ti_thread_flag(task_thread_info(tsk), TIF_SIGPENDING);
		if (tsk->state != TASK_RUNNING)
			return wake_up_process(tsk);
	}

	return 0;
}

int send_rt_signal(struct task_struct *tsk, int sig, int value)
{
	struct sigqueue *q;

	q = kmem_cache_alloc(sigqueue_cache, CACHE_OPT_NONE);
	if (!q)
		return -1;
	q->flags = 0;
	q->info.si_signo = sig;
	q->info._rt.si_pid = current->pid;
	q->info._rt.si_value.sival_int = value;
	send_signal_info(sig, q, tsk);

	return 0;
}

void purge_pending_signals(struct task_struct *tsk)
{
	struct sigqueue *q, *n;

	list_for_each_entry_safe(q, n, &tsk->pending.list, list) {
		list_del(&q->list);
		if (!(q->flags & SIGQUEUE_PREALLOC))
			kmem_cache_free(sigqueue_cache, q);
	}
}

void do_signal(void)
{
	int sig;
	struct sigqueue *q;

	/* si_signo, si_errno and si_code are defined for all signals, as part
	 * of the siginfo_t struct. Although, there will be no siginfo_t struct
	 * for SIKILL and SIGSTOP, since they cannot be caught, ignored, or
	 * blocked. */
	if (sigismember(&current->pending.signal, SIGKILL))
		do_exit(SIGKILL);

	q = list_first_entry(&current->pending.list, struct sigqueue, list);
	sig = q->info.si_signo;
	sigdelset(&current->pending.signal, sig);
	if (!sig_ignore(current, sig))
		__do_signal(sig, q);
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

	if (sig_kernel_only(sig)) {
		switch (sig) {
		case SIGKILL:
			/* process SIGKILL early */
			tsk->signal->group_exit_code = SIGKILL;
			tsk->signal->flags = SIGNAL_GROUP_EXIT;
			zap_all_threads(tsk);
			return 0;
		case SIGSTOP:
		default:
			/* unhandled! */
			BUG();
		}
	}

	/* it's ok to have no handlers installed */
	if (!tsk->sighand)
		return 0;
	if (!tsk->sighand->action[sig - 1].sa_handler)
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
	off = sizeof(struct pt_regs);
	struct sigaction *act = task_sigaction(current, sig->info.si_signo);
	if (act->sa_flags & SA_SIGINFO)
		off += align_next(sizeof(siginfo_t), 8);
	current_thread_info()->user.psp += off;

	list_del(&sig->list);
	if (!(sig->flags & SIGQUEUE_PREALLOC))
		kmem_cache_free(sigqueue_cache, sig);

	/* If the interrupted task was in a syscall, this restores the
	 * syscall's return value. If it was interrupted because of an
	 * external interrupt, this restores the r0 value at the time of
	 * the interrupt. */
	return current_thread_info()->user.regs->r0;
}

SYSCALL_DEFINE(sigprocmask,
	int		how,
	const sigset_t	*set,
	sigset_t	*oldset)
{
	sigset_t _set, tmpset;

	if (oldset)
		memcpy(oldset, &current->blocked, sizeof(*oldset));

	/* Attempts to block SIGKILL or SIGSTOP are silently ignored */
	memcpy(&_set, set, sizeof(_set));
	sigdelset(&_set, SIGKILL);
	sigdelset(&_set, SIGSTOP);

	switch (how) {
	case SIG_BLOCK:
		sigorsets(&current->blocked, &current->blocked, &_set);
		break;
	case SIG_UNBLOCK:
		sigandsets(&tmpset, &current->pending.signal, &_set);
		if (!sigisemptyset(&tmpset))
			set_ti_thread_flag(current_thread_info(), TIF_SIGPENDING);
		sigandnsets(&current->blocked, &current->blocked, &_set);
		break;
	case SIG_SETMASK:
		memcpy(&current->blocked, &_set, sizeof(_set));
		break;
	default:
		/* errno = EINVAL; */
		return -1;
	}

	return 0;
}

SYSCALL_DEFINE(pause, void)
{
	sched_dequeue(current);
	set_current_state(TASK_INTERRUPTIBLE);

	schedule();

	//FIXME: Only signals can wake-up this task

	return -EINTR;
}

int signal_init(void)
{
	signal_struct_cache = KMEM_CACHE(signal_struct);
	BUG_ON(!signal_struct_cache);

	sigqueue_cache = KMEM_CACHE(sigqueue);
	BUG_ON(!sigqueue_cache);

	return 0;
}
