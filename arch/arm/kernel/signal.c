/*
 * arch/arm/kernel/signal.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/stddef.h>

#include <asm/current.h>
#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

#define __process_alloca_with_align(__ptr, __align) ({		\
	struct thread_info *__thrd = current_thread_info();	\
	u32 __sp = __thrd->user.psp;				\
	__sp -= sizeof(__typeof__(*(__ptr)));			\
	__sp = align(__sp, __align);				\
	__thrd->user.psp = __sp;				\
	__ptr = (__typeof__(__ptr))__sp; })

#define __process_alloca(__ptr) __process_alloca_with_align(__ptr, 1)

static void setup_sigframe(int sig, struct sigaction *sa, int value)
{
	siginfo_t *siginfo = NULL;
	struct cpu_user_context *sigctx;

	if (sa->sa_flags & SA_SIGINFO) {
		__process_alloca(siginfo);
		siginfo->si_signo = sig;
		siginfo->si_value.sival_int = value;
		siginfo->si_pid = current->tgid;
	}

	__process_alloca_with_align(sigctx, 8);
	sigctx->r0 = sig;          /* signum */
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

	/* unset the TIF_SIGPENDING flags */
	current_thread_info()->flags = 0;
}

static int do_signal(int sig, int value)
{
	struct sigaction *act;

	act = &current->sighand->action[sig];
	setup_sigframe(sig, act, value);

	return 0;
}

void do_notify_resume(int syscall_retval)
{
	struct thread_info *ti = current_thread_info();

	/* save the syscall return value into the syscall frame */
	ti->user.ctx->r0 = syscall_retval;

	if (/* thread_info_flags */ ti->flags & _TIF_SIGPENDING)
		do_signal(current->sigpending, current->sigval);
}

void do_notify(void)
{
	if (current_thread_info()->flags & _TIF_SIGPENDING)
		do_signal(current->sigpending, current->sigval);
}
