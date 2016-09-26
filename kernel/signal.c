/*
 * kernel/signal.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <kernel/bitops.h>
#include <kernel/signal.h>
#include <kernel/thread.h>

#include "kernel.h"
#include "cmsis/arm/ARMCM4.h"

extern void return_from_sighandler(void);
extern void return_from_sigaction(void);

void *v7m_alloca_thread_context(struct thread_info *tip, size_t len)
{
	tip->ti_mach.mi_psp -= len;

	return (void *)tip->ti_mach.mi_psp;
}

void v7m_push_thread_context(struct thread_info *tip, void *data, size_t len)
{
	void *stack_pointer = v7m_alloca_thread_context(tip, len);

	memcpy(stack_pointer, data, len);
}

static inline void *v7m_set_thumb_bit(void *addr)
{
    return (void *)((unsigned long)addr | 1ul);
}

static inline void *v7m_clear_thumb_bit(void *addr)
{
    return (void *)((unsigned long)addr & ~1ul);
}

static void stage_sighandler(struct sigaction *sigaction, int sig)
{
	CURRENT_THREAD_INFO(threadp);
	struct thread_context_regs *tcr;

	printk("signal(%d): Staging handler %p\n", sig, sigaction->sa_handler);

	/* SP_process for the Current thread control block has not been updated,
	   need to update it because we are pushing data to the process stack.  */
	threadp->ti_mach.mi_psp = __get_PSP();

	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(threadp, sizeof(struct thread_context_regs));

	/* build the sigaction trampoline */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
	tcr->r0_r3__r12[0] = sig;
	tcr->r0_r3__r12[1] = 0;
	tcr->r0_r3__r12[2] = 0;
	tcr->r0_r3__r12[3] = 0;
	tcr->r0_r3__r12[4] = 0;
	tcr->lr = (u32)v7m_set_thumb_bit(return_from_sighandler);
	tcr->ret_addr = (u32)v7m_clear_thumb_bit(sigaction->sa_handler);
	tcr->xpsr = xPSR_T_Msk;

	/* We staged the sigaction on the current thread context, so update the
	   SP_process before returning to thread.  */
	__set_PSP(threadp->ti_mach.mi_psp);
}

static void stage_sigaction(const struct sigaction *sigaction, int sig,
			union sigval value)
{
	CURRENT_THREAD_INFO(threadp);
	struct thread_context_regs *tcr;
	siginfo_t *siginfo_ptr;

	printk("signal(%d): Staging sigaction %p\n", sig, sigaction->sa_sigaction);

	/* SP_process for the Current thread control block has not been updated,
	   need to update it because we are pushing data to the process stack.  */
	threadp->ti_mach.mi_psp = __get_PSP();

	/* allocate the siginfo_t struct on thread's stack (SP_process); that
	   memory will be reclaimed during return_from_sigaction */
	siginfo_ptr = v7m_alloca_thread_context(threadp, sizeof(siginfo_t));
	siginfo_ptr->si_signo = sig;
	siginfo_ptr->si_value = value;
	siginfo_ptr->si_pid = threadp->ti_id;

	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(threadp, sizeof(struct thread_context_regs));

	/* build the sigaction trampoline */ /* build_context() */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
	tcr->r0_r3__r12[0] = sig;
	tcr->r0_r3__r12[1] = (u32)siginfo_ptr; /* XXX: allocate on the process stack?
						  and cleanup on return_from_sigaction? */
	tcr->r0_r3__r12[2] = 0;  /* POSIX says it's a ucontext_t *, but commonly unused */
	tcr->r0_r3__r12[3] = 0;
	tcr->r0_r3__r12[4] = 0;
	tcr->lr = (u32)v7m_set_thumb_bit(return_from_sigaction);
	tcr->ret_addr = (u32)v7m_clear_thumb_bit(sigaction->sa_sigaction);
	tcr->xpsr = xPSR_T_Msk;

	/* We staged the sigaction on the current thread context, so update the
	   SP_process before returning to thread.  */
	__set_PSP(threadp->ti_mach.mi_psp);
}

/* static stage_sigevent(); ... */

static void do_sigevent(const struct sigevent *sigevent)
{
	CURRENT_THREAD_INFO(threadp);
	struct thread_context_regs *tcr;

	//printk("signal(%d): Staging sigaction %p\n", sig, sigaction->sa_sigaction);

	//if (sigevent->sigev_notify == SIGEV_THREAD) {

	/* SP_process for the Current thread control block has not been updated,
	   need to update it because we are pushing data to the process stack.  */
	threadp->ti_mach.mi_psp = __get_PSP();

	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(threadp, sizeof(struct thread_context_regs));

	/* build the sigaction trampoline */ /* build_context() */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
	tcr->r0_r3__r12[0] = sigevent->sigev_value.sival_int;
	tcr->r0_r3__r12[1] = 0;
	tcr->r0_r3__r12[2] = 0;
	tcr->r0_r3__r12[3] = 0;
	tcr->r0_r3__r12[4] = 0;
	tcr->lr = (u32)v7m_set_thumb_bit(return_from_sighandler);
	tcr->ret_addr = (u32)v7m_clear_thumb_bit(sigevent->sigev_notify_function);
	tcr->xpsr = xPSR_T_Msk;

	/* We staged the sigaction on the current thread context, so update the
	   SP_process before returning to thread.  */
	__set_PSP(threadp->ti_mach.mi_psp);
}

static struct sigaction *find_sigaction_by_sig(struct thread_info *tip, int sig)
{
	struct ksignal *kact;

	list_for_each_entry(kact, &tip->ti_sigactions, ksig_list) {
		if (kact->ksig_signo == sig)
			return &kact->ksig_struct;
	}

	return NULL;
}

int __sigaction(int sig, const struct sigaction *restrict act,
		struct sigaction *restrict oact)
{
	struct sigaction *old_act;
	struct ksignal *ksig;

	CURRENT_THREAD_INFO(threadp);

	if ((sig == SIGKILL) || (sig == SIGSTOP)) {
		//erno = EINVAL;
		return -1;
	}

	if (oact) {
		old_act = find_sigaction_by_sig(threadp, sig);
		if (old_act != NULL)
			memcpy(oact, old_act, sizeof(struct sigaction));
	}

	ksig = malloc(sizeof(struct ksignal));
	if (ksig == NULL)
		return -1;
	ksig->ksig_signo = sig;
	list_add(&ksig->ksig_list, &threadp->ti_sigactions);
	memcpy(&ksig->ksig_struct, act, sizeof(struct sigaction));

	return 0;
}

static unsigned long supported_signal_mask = (1 << SIGKILL) | (1 << SIGUSR1)
	| (1 << SIGUSR2) | (1 << SIGSTOP);

static int is_signal_supported(int sig)
{
	if (sig > SIGMAX)
		return 0;
	return bitmap_get_bit(&supported_signal_mask, sig);
}

int __raise(int sig)
{
	struct sigaction *act;
	CURRENT_THREAD_INFO(threadp);

	if (!is_signal_supported(sig))
		return ERR_SIGNAL_UNSUPPORTED;
	act = find_sigaction_by_sig(threadp, sig);
	if (act == NULL)
		return ERR_SIGNAL_UNHANDLED;
	if (act->sa_flags & SA_SIGINFO)
		stage_sigaction(act, sig, (union sigval){ .sival_int = 0 });
	else
		stage_sighandler(act, sig);

	return 0; //FIXME: OFFSET TO RETVAL???
}

int __do_sigqueue(pid_t pid, int sig, const union sigval value,
		unsigned long *offset_to_retval)
{
	int err = 0;
	struct sigaction *act;
	CURRENT_THREAD_INFO(threadp);

	if (!is_signal_supported(sig)) {
		err = ERR_SIGNAL_UNSUPPORTED;
		goto out;
	}

	//FIXME: we don't handle staging to a different PID
	pid = (pid_t)threadp->ti_id;

	act = find_sigaction_by_sig(threadp, sig);
	if (act == NULL) {
		err = ERR_SIGNAL_UNHANDLED;
		goto out;
	}

	//FIXME: must check if a handler with SA_SIGINFO has been installed
	//FIXME: stage on supplied task/thread id, use pid...
	//FIXME: must check there is enough space on user thread stack
	stage_sigaction(act, sig, value);

	/* r0 can be clobbered by the signal handling function */
out:
	if (err || (pid != (pid_t)threadp->ti_id))
		*offset_to_retval = 0;
	else
		*offset_to_retval = sizeof(struct thread_context_regs) + sizeof(siginfo_t);

	return err;
}
