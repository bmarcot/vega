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
#include <kernel/thread.h>

#include <asm/current.h>
#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

#include <uapi/kernel/signal.h>

void *v7m_alloca_thread_context(struct thread_info *tip, size_t len)
{
	tip->thread_ctx.sp -= len;

	return (void *)tip->thread_ctx.sp;
}

void v7m_push_thread_context(struct thread_info *tip, void *data, size_t len)
{
	void *stack_pointer = v7m_alloca_thread_context(tip, len);

	memcpy(stack_pointer, data, len);
}

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

int sys_sigaction(int signum, const struct sigaction *act,
		struct sigaction *oldact)
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

static inline void init_sigctx(struct cpu_saved_context *sigctx, u32 arg1,
			u32 arg2, struct sigaction *sa)
{
	sigctx->r1 = arg1; /* siginfo_t or nil */
	sigctx->r2 = arg2; /* ucontext_t *, but commonly unused */
	sigctx->r3 = 0;
	sigctx->r12 = 0;
	if (sa->sa_flags & SA_RESTORER)
		sigctx->lr = (u32)v7m_set_thumb_bit(sa->sa_restorer);
	else
		sigctx->lr = 0;
	sigctx->ret_addr = (u32)v7m_clear_thumb_bit(sa->sa_handler);
	sigctx->xpsr = xPSR_T_Msk;
}

static void do_handler(struct sigaction *sa)
{
	v7m_alloca_thread_context(current_thread_info(),
				sizeof(struct cpu_saved_context));
	struct cpu_saved_context *sigctx =
		current_thread_info()->thread_ctx.ctx;
	init_sigctx(sigctx, 0, 0, sa);
}

static void do_sigaction(int sig, struct sigaction *sa, union sigval value)
{
	v7m_alloca_thread_context(current_thread_info(),
				align_next(sizeof(siginfo_t), 8));
	siginfo_t *siginfop =
		(siginfo_t *)current_thread_info()->thread_ctx.sp;
	siginfop->si_signo = sig;
	siginfop->si_value = value;
	siginfop->si_pid = current->pid;

	v7m_alloca_thread_context(current_thread_info(),
				sizeof(struct cpu_saved_context));
	struct cpu_saved_context *sigctx =
		current_thread_info()->thread_ctx.ctx;
	init_sigctx(sigctx, (u32)siginfop, 0, sa);
}

static int do_sigqueue(__unused pid_t pid, int sig, union sigval value)
{
	struct ksignal *ks = get_ksignal(sig);
	if (!ks)
		return -EINVAL;

	current->sig = sig;
	if (ks->sa.sa_flags & SA_SIGINFO)
		do_sigaction(sig, &ks->sa, value);
	else
		do_handler(&ks->sa);

	return sig;
}

int sys_sigqueue(pid_t pid, int sig, union sigval value)
{
	return do_sigqueue(pid, sig, value);
}

int sys_kill(pid_t pid, int sig)
{
	return do_sigqueue(pid, sig, (union sigval){0});
}

int sys_sigreturn(void)
{
	struct ksignal *ks = get_ksignal(current->sig);

	int off = sizeof(struct cpu_saved_context);
	if (ks->sa.sa_flags & SA_SIGINFO)
		off += align_next(sizeof(siginfo_t), 8);
	current_thread_info()->thread_ctx.sp += off;
	current->sig = -1;

	/* this is the actual return value to the kill() syscall */
	return 0;
}

void do_sigevent(struct task_struct *tsk, struct sigevent *sigevent)
{
	(void)tsk;

	if (sigevent->sigev_notify != SIGEV_SIGNAL)
		return;
	do_sigqueue(0, sigevent->sigev_signo, sigevent->sigev_value);
}
