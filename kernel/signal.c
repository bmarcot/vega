/*
 * kernel/signal.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>

#include <asm/current.h>
#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

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
	if (!current->sighand) {
		sighand = kzalloc(sizeof(*sighand));
		if (!sighand)
			return -1;
		current->sighand = sighand;
	} else {
		sighand = current->sighand;
	}

	/* save and install a new handler */
	k_act = &sighand->action[signum];
	if (oldact)
		memcpy(oldact, k_act, sizeof(*k_act));
	memcpy(k_act, act, sizeof(*k_act));

	return 0;
}

#define __USER_STACK_ALLOCA(__ptr, __align) ({		\
	u32 __sp = current_thread_info()->user.psp;	\
	__sp -= sizeof(__typeof__(*(__ptr)));		\
	__sp = align(__sp, __align);			\
	current_thread_info()->user.psp = __sp;		\
	__ptr = (__typeof__(__ptr))__sp; })

static void __send_signal(int sig, struct sigaction *sa, union sigval value)
{
	siginfo_t *siginfo = NULL;
	struct cpu_user_context *sigctx;

	if (sa->sa_flags & SA_SIGINFO) {
		__USER_STACK_ALLOCA(siginfo, 4);
		siginfo->si_signo = sig;
		siginfo->si_value = value;
		siginfo->si_pid = current->tgid;
	}

	__USER_STACK_ALLOCA(sigctx, 8);
	sigctx->r1 = (u32)siginfo; /* siginfo_t or nil */
	sigctx->r2 = 0;            /* ucontext_t *, but commonly unused */
	sigctx->r3 = 0;
	sigctx->r12 = 0;
	if (sa->sa_flags & SA_RESTORER)
		sigctx->lr = (u32)v7m_set_thumb_bit(sa->sa_restorer);
	else
		sigctx->lr = 0;
	sigctx->ret_addr = (u32)v7m_clear_thumb_bit(sa->sa_handler);
	sigctx->xpsr = xPSR_T_Msk;
}

static int send_signal(__unused pid_t pid, int sig, union sigval value)
{
	struct sigaction *act;

	/* no handlers have been installed */
	if (!current->sighand)
		return -EINVAL;

	act = &current->sighand->action[sig];

	/* no handler for that signal */
	if (!act->sa_handler)
		return -EINVAL;

	current->sigpending = sig;
	__send_signal(sig, act, value);

	return sig;
}

SYSCALL_DEFINE(sigqueue,
	pid_t		pid,
	int		sig,
	union sigval	value)
{
	return send_signal(pid, sig, value);
}

SYSCALL_DEFINE(kill,
	pid_t		pid,
	int		sig)
{
	return send_signal(pid, sig, (union sigval){0});
}

SYSCALL_DEFINE(sigreturn, void)
{
	struct sigaction *act;
	int off;

	if (current->sigpending == -1) {
		pr_warn("Wants to return from signal, but no signal raised");
		return 0;
	}

	act = &current->sighand->action[current->sigpending];
	off = sizeof(struct cpu_user_context);
	if (act->sa_flags & SA_SIGINFO)
		off += align_next(sizeof(siginfo_t), 8);
	current_thread_info()->user.psp += off;
	current->sigpending = -1;

	/* this is the actual return value to the kill() syscall */
	//FIXME: If sending signal to self
	return 0;
}

void do_sigevent(struct task_struct *tsk, struct sigevent *sigevent)
{
	(void)tsk;

	if (sigevent->sigev_notify != SIGEV_SIGNAL)
		return;
	send_signal(0, sigevent->sigev_signo, sigevent->sigev_value);
}
