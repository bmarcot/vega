/*
 * kernel/signal.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */


#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <arch/v7m-helper.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/signal.h>
#include <kernel/task.h>
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

static void stage_sighandler(struct sigaction *sigaction)
{
	CURRENT_THREAD_INFO(threadp);
	struct thread_context_regs *tcr;

	/* SP_process for the Current thread control block has not been updated,
	   need to update it because we are pushing data to the process stack.  */
	threadp->ti_mach.mi_psp = __get_PSP();

	/* this is the exception stacked-context */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
	tcr->r0_r3__r12[0] = 0; /* return value of syscall, cannot fail after this point */

	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(threadp, sizeof(struct thread_context_regs));

	/* build the sigaction trampoline */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
/* #ifdef SECURE_KERNEL */
	tcr->r0_r3__r12[1] = 0;
	tcr->r0_r3__r12[2] = 0;
	tcr->r0_r3__r12[3] = 0;
	tcr->r0_r3__r12[4] = 0;
/* #endif */
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

	/* SP_process for the Current thread control block has not been updated,
	   need to update it because we are pushing data to the process stack.  */
	threadp->ti_mach.mi_psp = __get_PSP();

	/* this is the exception stacked-context */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
	tcr->r0_r3__r12[0] = 0; /* return value of syscall, cannot fail after this point */

	/* allocate the siginfo_t struct on thread's stack (SP_process); that
	   memory will be reclaimed during return_from_sigaction */
	siginfo_t *siginfo_ptr = v7m_alloca_thread_context(threadp, sizeof(siginfo_t));
	siginfo_ptr->si_signo = sig;
	siginfo_ptr->si_value = value;
	siginfo_ptr->si_pid = threadp->ti_id;

	/* the sigaction context will be poped by cpu on exception return */
	v7m_alloca_thread_context(threadp, sizeof(struct thread_context_regs));

	/* build the sigaction trampoline */ /* build_context() */
	tcr = (struct thread_context_regs *)threadp->ti_mach.mi_psp;
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

void do_sigevent(const struct sigevent *sigevent)
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

extern struct task_info top_task;

static struct sigaction *find_sigaction_by_sig(pid_t pid, int sig)
{
	(void)pid; //XXX: Multi-tasking not implemented yet

	struct ksignal *ks;
	list_for_each_entry(ks, &top_task.signal_head, ksig_list) {
		if (ks->ksig_signo == sig)
			return &ks->ksig_struct;
	}

	return NULL;
}

int sys_sigaction(int sig, const struct sigaction *restrict act,
		struct sigaction *restrict oldact)
{
	if ((sig == SIGKILL) || (sig == SIGSTOP)) {
		errno = EINVAL;
		return -1;
	}
	if (act == NULL) {
		errno = EFAULT;
		return -1;
	}

	if (oldact) {
		struct sigaction *oact = find_sigaction_by_sig(0, sig);
		if (oact != NULL)
			memcpy(oldact, oact, sizeof(struct sigaction));
	}

	struct ksignal *ksignal = malloc(sizeof(struct ksignal));
	if (ksignal == NULL) {
		errno = ENOMEM;
		return -1;
	}

	ksignal->ksig_signo = sig;
	list_add(&ksignal->ksig_list, &top_task.signal_head);
	memcpy(&ksignal->ksig_struct, act, sizeof(struct sigaction));

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

int sys_raise(int sig)
{
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
