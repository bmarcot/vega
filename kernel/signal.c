/*
 * kernel/signal.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/thread.h>

#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

#include <uapi/kernel/signal.h>

#include "kernel.h"
#include "platform.h"

extern void return_from_sighandler(void);

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

void do_sigevent(const struct sigevent *sigevent, struct thread_info *thread)
{
	CURRENT_THREAD_INFO(curr_thread);
	struct cpu_saved_context *ctx;

	//if (sigevent->sigev_notify == SIGEV_THREAD) {

	/* update current thread SP_process */
	if (thread == curr_thread)
		thread->thread_ctx.sp = __get_PSP();

	/* the sigevent context will be poped by cpu on exception return */
	v7m_alloca_thread_context(thread, sizeof(struct cpu_saved_context));

	/* build a sigevent trampoline */
	ctx = thread->thread_ctx.ctx;
	ctx->r0 = sigevent->sigev_value.sival_int;
	ctx->r1 = 0;
	ctx->r2 = 0;
	ctx->r3 = 0;
	ctx->r12 = 0;
	ctx->lr = (__u32)v7m_set_thumb_bit(return_from_sighandler);
	ctx->ret_addr =
		(__u32)v7m_clear_thumb_bit(sigevent->sigev_notify_function);
	ctx->xpsr = xPSR_T_Msk;

	/* update current thread SP_process */
	if (thread == curr_thread)
		__set_PSP(thread->thread_ctx.sp);
}

/* new signals */

#include <asm/current.h>

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
	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(current_thread_info(),
				sizeof(struct cpu_saved_context));

	struct cpu_saved_context *sigctx =
		current_thread_info()->thread_ctx.ctx;

	/* build the sigaction trampoline */
	sigctx->r1 = 0;
	sigctx->r2 = 0;
	sigctx->r3 = 0;
	sigctx->r12 = 0;
	if (sa->sa_flags & SA_RESTORER)
		sigctx->lr = (u32)v7m_set_thumb_bit(sa->sa_restorer);
	else
		sigctx->lr = 0;
	sigctx->ret_addr = (u32)v7m_clear_thumb_bit(sa->sa_handler);
	sigctx->xpsr = xPSR_T_Msk;
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

	/* build a sigaction trampoline */
	sigctx->r1 = (u32)siginfop;
	sigctx->r2 = 0; /* ucontext_t *, but commonly unused */
	sigctx->r3 = 0;
	sigctx->r12 = 0;
	if (sa->sa_flags & SA_RESTORER)
		sigctx->lr = (u32)v7m_set_thumb_bit(sa->sa_restorer);
	else
		sigctx->lr = 0;
	sigctx->ret_addr = (u32)v7m_clear_thumb_bit(sa->sa_handler);
	sigctx->xpsr = xPSR_T_Msk;
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
	if (ks->sa.sa_flags & SA_SIGINFO) {
		off += align_next(sizeof(siginfo_t), 8);
		pr_info("has SA_SIGINFO");
	}
	current_thread_info()->thread_ctx.sp += off;
	current->sig = -1;

	/* this is the actual return value to the kill() syscall */
	return 0;
}
