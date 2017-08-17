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
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>

#include <asm/current.h>
#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

//FIXME: this is part of the task_struct->signal_struct
static LIST_HEAD(ksignals); /* list of installed handlers */

static struct ksignal *get_ksignal(int sig)
{
	struct ksignal *ks;

	list_for_each_entry(ks, &ksignals, list) {
		if (ks->sig == sig)
			return ks;
	}

	return NULL;
}

SYSCALL_DEFINE(sigaction,
	int			signum,
	const struct sigaction	*act,
	struct sigaction	*oldact)
{
	if ((signum == SIGKILL) || (signum == SIGSTOP)) {
		errno = EINVAL;
		return -1;
	}

	/* do checks on act variable: is address valid, not nil, etc. */
	if (!act) {
		errno = EFAULT;
		return -1;
	}

	/* lookup fo a previous installed handler */
	struct ksignal *ks = get_ksignal(signum);

	//FIXME: is user-supplied address valid?
	if (ks && oldact)
		memcpy(oldact, ks, sizeof(struct sigaction));

	if (!ks) {
		ks = malloc(sizeof(struct ksignal));
		if (!ks) {
			errno = ENOMEM;
			return -1;
		}
	}

	ks->sig = signum;
	memcpy(&ks->sa, act, sizeof(struct sigaction));
	list_add(&ks->list, &ksignals);

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
		siginfo->si_pid = current->pid;
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
	struct ksignal *ks = get_ksignal(sig);
	if (!ks)
		return -EINVAL;

	current->sig = sig;
	__send_signal(sig, &ks->sa, value);

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
	struct ksignal *ks = get_ksignal(current->sig);

	int off = sizeof(struct cpu_user_context);
	if (ks->sa.sa_flags & SA_SIGINFO)
		off += align_next(sizeof(siginfo_t), 8);
	current_thread_info()->user.psp += off;
	current->sig = -1;

	/* this is the actual return value to the kill() syscall */
	return 0;
}

void do_sigevent(struct task_struct *tsk, struct sigevent *sigevent)
{
	(void)tsk;

	if (sigevent->sigev_notify != SIGEV_SIGNAL)
		return;
	send_signal(0, sigevent->sigev_signo, sigevent->sigev_value);
}
