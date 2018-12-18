/*
 * arch/arm/kernel/signal.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <string.h>

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

static void setup_sigframe(struct sigqueue *q, struct sigaction *sa)
{
	siginfo_t *siginfo = NULL;
	struct pt_regs *regs;

	if (sa->sa_flags & SA_SIGINFO) {
		__process_alloca(siginfo);
		memcpy(siginfo, &q->info, sizeof(*siginfo));
	}

	__process_alloca_with_align(regs, 8);
	regs->r0 = q->info.si_signo;	/* signum */
	regs->r1 = (u32)siginfo;	/* siginfo_t or null */
	regs->r2 = 0;			/* ucontext_t *, commonly unused */
	if (sa->sa_flags & SA_RESTORER)
		regs->lr = (u32)v7m_set_thumb_bit(sa->sa_restorer);
	else
		regs->lr = 0;
	regs->pc = (u32)v7m_clear_thumb_bit(sa->sa_handler);
	regs->status = xPSR_T_Msk;
}

void __do_signal(int signo, struct sigqueue *sig)
{
	clear_thread_flag(TIF_SIGPENDING);
	setup_sigframe(sig, &current->sighand->action[signo - 1]);
}

void do_notify_resume(int syscall_retval)
{
	/* Save the return value that will be restored by sigreturn() */
	current_thread_info()->user.regs->r0 = syscall_retval;

	if (test_thread_flag(TIF_SIGPENDING))
		do_signal();
	if (test_thread_flag(TIF_NEED_RESCHED))
		schedule();
}

void do_notify(void)
{
	if (test_thread_flag(TIF_SIGPENDING))
		do_signal();
}
