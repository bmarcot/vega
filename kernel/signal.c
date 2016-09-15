/*
 * kernel/signal.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <signal.h>
#include <string.h>

#include <kernel/thread.h>

#include "kernel.h"
#include "cmsis/arm/ARMCM4.h"

extern void return_from_sigaction(void);

static void stage_sigaction(struct sigaction *sigaction, int sig)
{
	CURRENT_THREAD_INFO(threadp);
	struct thread_context_regs *tcr;

	printk("signal(%d): Staging %p\n", sig, sigaction->sa_handler);

	/* SP_process for the Current thread control block has not been updated,
	   need to update it because we are pushing data to the process stack.  */
	threadp->ti_mach.mi_psp = __get_PSP();

	/* the sigaction context will be poped by cpu on exception return */
	threadp->ti_mach.mi_psp -= sizeof(struct thread_context_regs);

	/* build the sigaction trampoline */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
	tcr->r0_r3__r12[0] = sig;
	tcr->r0_r3__r12[1] = 0;
	tcr->r0_r3__r12[2] = 0;
	tcr->r0_r3__r12[3] = 0;
	tcr->r0_r3__r12[4] = 0;
	tcr->lr = (u32)return_from_sigaction | 1;  /* return in Thumb Mode */
	tcr->ret_addr = (u32)sigaction->sa_handler & 0xfffffffe;
	tcr->xpsr = xPSR_T_Msk;

	/* We staged the sigaction on the current thread context, so update the
	   SP_process before returning to thread.  */
	__set_PSP(threadp->ti_mach.mi_psp);
}

int __sigaction(int sig, const struct sigaction *restrict act,
		struct sigaction *restrict oact)
{
	CURRENT_THREAD_INFO(threadp);

	if ((sig == SIGKILL) || (sig == SIGSTOP)) {
		//erno = EINVAL;
		return -1;
	}

	if (oact)
		memcpy(oact, &threadp->ti_sigactions[sig], sizeof(struct sigaction));

	if (act->sa_flags & SA_SIGINFO) {
		//FIXME: we handle sa_handler only, at the moment
		;
	} else {
		memcpy(&threadp->ti_sigactions[sig], act, sizeof(struct sigaction));
	}

	return 0;
}

int __raise(int sig)
{
	CURRENT_THREAD_INFO(threadp);
	stage_sigaction(&threadp->ti_sigactions[sig], sig);

	return 0;
}
