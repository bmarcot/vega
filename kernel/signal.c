/*
 * kernel/signal.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <signal.h>
#include <string.h>
#include <sys/types.h>

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
	tcr->lr = (u32)return_from_sighandler | 1 ;  /* return in Thumb Mode */
	tcr->ret_addr = (u32)sigaction->sa_handler & 0xfffffffe;
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
	tcr->lr = (u32)return_from_sigaction | 1;  /* return in Thumb Mode */
	tcr->ret_addr = (u32)sigaction->sa_sigaction & 0xfffffffe;
	tcr->xpsr = xPSR_T_Msk;

	/* We staged the sigaction on the current thread context, so update the
	   SP_process before returning to thread.  */
	__set_PSP(threadp->ti_mach.mi_psp);
}

/* static stage_sigevent(); ... */

static struct sigaction *find_sigaction_by_sig(struct thread_info *tip, int sig)
{
	struct sigaction *act;

	list_for_each_entry(act, &tip->ti_sigactions, sa_list) {
		if (act->sa_signo == sig)
			return act;
	}

	return NULL;
}

/* This prototype differs from the actual POSIX standard protoype.  Removing the
   const qualifier on `act' variable greatly reduces the pain to allocate the
   memory storage.  If the const qualifier was to be kept; the kernel would need
   to dynamically allocate some storage for the sigaction struct.  */
/* This is a tradeoff between standard compliance and speed of execution.  */
/* int __sigaction(int sig, const struct sigaction *restrict act, */
/* 		struct sigaction *restrict oact) */
int __sigaction(int sig, struct sigaction *act, struct sigaction *restrict oact)
{
	struct sigaction *old_act;
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
	act->sa_signo = sig;
	list_add(&act->sa_list, &threadp->ti_sigactions);

	return 0;
}

int __raise(int sig)
{
	struct sigaction *act;
	CURRENT_THREAD_INFO(threadp);

	act = find_sigaction_by_sig(threadp, sig);
	if (act == NULL)
		return -1;
	stage_sighandler(act, sig);

	if (act->sa_flags & SA_SIGINFO)
		stage_sigaction(act, sig, (union sigval){ .sival_int = 0 });
	else
		stage_sighandler(act, sig);

	return 0;
}

int __sigqueue(pid_t pid, int sig, const union sigval value)
{
	(void)pid;

	struct sigaction *act;
	CURRENT_THREAD_INFO(threadp);

	act = find_sigaction_by_sig(threadp, sig);
	if (act == NULL)
		return -1;

	//FIXME: must check if a handler with SA_SIGINFO has been installed

	//FIXME: stage on supplied task/thread id, use pid...
	stage_sigaction(act, sig, value);

	return 0;
}
