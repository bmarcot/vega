/*
 * arch/arm/kernel/signal.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/stddef.h>
#include <kernel/thread_info.h>

#include <asm/current.h>
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
	clear_thread_flag(TIF_SIGPENDING);
}

void do_exit(int status);

static int do_signal(void)
{
	struct sigaction *act;
	struct sigqueue *sig = list_first_entry(&current->pending.list,
						struct sigqueue, list);
	int signo = sig->info.si_signo;

	if (signo == SIGKILL)
		do_exit(0);

	act = &current->sighand->action[signo];
	setup_sigframe(signo, act, sig->info.si_value.sival_int);

	return 0;
}

void do_notify_resume(int syscall_retval)
{
	/* save the syscall return value into the syscall frame */
	current_thread_info()->user.ctx->r0 = syscall_retval;

	if (test_thread_flag(TIF_SIGPENDING))
		do_signal();
}

void do_notify(void)
{
	if (test_thread_flag(TIF_SIGPENDING))
		do_signal();
}
