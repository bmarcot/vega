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

#include <arch/v7m-helper.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/thread.h>

#include <asm/thread_info.h>

#include "kernel.h"
#include "platform.h"

static LIST_HEAD(signal_head); /* list of installed handlers */

extern void return_from_sighandler(void);
extern void return_from_sigaction(void);

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

static void stage_sighandler(struct sigaction *sigaction)
{
	CURRENT_THREAD_INFO(curr_thread);
	struct cpu_saved_context *ctx;

	/* update current thread SP_process */
	curr_thread->thread_ctx.sp = __get_PSP();

	/* this is the exception stacked-context */
	ctx = curr_thread->thread_ctx.ctx;

	/* return value of syscall, cannot fail after this point */
	ctx->r0 = 0;

	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(curr_thread,
				sizeof(struct cpu_saved_context));

	/* build the sigaction trampoline */
	ctx = curr_thread->thread_ctx.ctx;
/* #ifdef SECURE_KERNEL */
	ctx->r1 = 0;
	ctx->r2 = 0;
	ctx->r3 = 0;
	ctx->r12 = 0;
/* #endif */
	ctx->lr = (__u32)v7m_set_thumb_bit(return_from_sighandler);
	ctx->ret_addr = (__u32)v7m_clear_thumb_bit(sigaction->sa_handler);
	ctx->xpsr = xPSR_T_Msk;

	/* update current thread SP_process */
	__set_PSP(curr_thread->thread_ctx.sp);
}

static void stage_sigaction(const struct sigaction *sigaction, int sig,
			union sigval value)
{
	CURRENT_THREAD_INFO(curr_thread);
	struct cpu_saved_context *ctx;

	/* update current thread SP_process */
	curr_thread->thread_ctx.sp = __get_PSP();

	/* this is the exception stacked-context */
	ctx = curr_thread->thread_ctx.ctx;

	/* return value of syscall, cannot fail after this point */
	ctx->r0 = 0;

	/* The siginfo_t struct is allocated on thread's stack; that memory
	 * will be reclaimed during return_from_sigaction. */
	siginfo_t *siginfo_ptr =
		v7m_alloca_thread_context(curr_thread, sizeof(siginfo_t));
	siginfo_ptr->si_signo = sig;
	siginfo_ptr->si_value = value;
	siginfo_ptr->si_pid = TASK_STRUCT(curr_thread)->pid;

	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(curr_thread,
				sizeof(struct cpu_saved_context));

	/* build a sigaction trampoline */
	ctx = curr_thread->thread_ctx.ctx;
	ctx->r1 = (u32)siginfo_ptr;
	ctx->r2 = 0; /* ucontext_t *, but commonly unused */
	ctx->r3 = 0;
	ctx->r12 = 0;
	ctx->lr = (u32)v7m_set_thumb_bit(return_from_sigaction);
	ctx->ret_addr = (u32)v7m_clear_thumb_bit(sigaction->sa_sigaction);
	ctx->xpsr = xPSR_T_Msk;

	/* update current thread SP_process */
	__set_PSP(curr_thread->thread_ctx.sp);
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

static struct sigaction *find_sigaction_by_sig(pid_t pid, int sig)
{
	(void)pid; //XXX: Multi-tasking not implemented yet

	struct signal_info *signal;

	list_for_each_entry(signal, &signal_head, list) {
		if (signal->signo == sig)
			return &signal->act_storage;
	}

	return NULL;
}

int sys_sigaction(int signo, const struct sigaction *restrict act,
		struct sigaction *restrict oldact)
{
	if ((signo == SIGKILL) || (signo == SIGSTOP)) {
		errno = EINVAL;
		return -1;
	}
	if (act == NULL) {
		errno = EFAULT;
		return -1;
	}

	if (oldact) {
		struct sigaction *oact = find_sigaction_by_sig(0, signo);
		if (oact != NULL)
			memcpy(oldact, oact, sizeof(struct sigaction));
	}

	struct signal_info *signal = malloc(sizeof(struct signal_info));
	if (signal == NULL) {
		errno = ENOMEM;
		return -1;
	}

	signal->signo = signo;
	list_add(&signal->list, &signal_head);
	memcpy(&signal->act_storage, act, sizeof(struct sigaction));

	return 0;
}

/* enabled signal mask */
static unsigned long supported_signal_mask = (1 << SIGKILL) | (1 << SIGUSR1)
	| (1 << SIGUSR2) | (1 << SIGSTOP);

static int is_signal_supported(int sig)
{
	if (sig > SIGMAX)
		return 0;
	return bitmap_get_bit(&supported_signal_mask, sig);
}

/* How signal works?
 *
 * - A fake exception return context is allocated to the user thread stack.
 * - This context is a trampoline to the signal handler.
 * - When the syscall handler returns, the return value is pushed to the user
 *   stack in r0.  For signal handling, r0 must contain the first parameter to
 *   the signal handler function.  The actual return code of the syscall must
 *   be written into the auto-pushed stack context.  The staging functions
 *   handle the update of the error code in the cpu-pushed stackframe.
 */

int sys_kill(pid_t pid, int sig)
{
	(void)pid;

	if (!is_signal_supported(sig))
		return -EINVAL;

	struct sigaction *act = find_sigaction_by_sig(0, sig);
	if (act == NULL)
		return -EINVAL;

	if (act->sa_flags & SA_SIGINFO)
		stage_sigaction(act, sig, (union sigval){ .sival_int = 0 });
	else
		stage_sighandler(act);

	return sig;
}

int sys_sigqueue(pid_t pid, int sig, const union sigval value)
{
	(void)pid; //XXX: Multi-tasking not implemented yet

	if (!is_signal_supported(sig)) {
		errno = EINVAL;
		return -1;
	}

	struct sigaction *act = find_sigaction_by_sig(0, sig);
	if (act == NULL) {
		errno = EINVAL;
		return -1;
	}

	//FIXME: must check there is enough space on user thread stack
	if (act->sa_flags & SA_SIGINFO)
		stage_sigaction(act, sig, value);
	else
		stage_sighandler(act);

	return sig;
}
