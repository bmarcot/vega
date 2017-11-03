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
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>

#include <asm/current.h>
#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

SYSCALL_DEFINE(sigaction,
	int			signum,
	const struct sigaction	*act,
	struct sigaction	*oldact)
{
	struct sighand_struct *sighand;
	struct sigaction *k_act;

	if ((signum == SIGKILL) || (signum == SIGSTOP)) {
		errno = EINVAL;
		return -1;
	}

	/* do checks on act variable: is address valid, not nil, etc. */
	if (!act) {
		errno = EFAULT;
		return -1;
	}

	/* alloc or get the signal handler table */
	if (!current->sighand) {
		sighand = kzalloc(sizeof(*sighand));
		if (!sighand)
			return -1;
		current->sighand = sighand;
	} else {
		sighand = current->sighand;
	}

	/* save and install a new handler */
	k_act = &sighand->action[signum];
	if (oldact)
		memcpy(oldact, k_act, sizeof(*k_act));
	memcpy(k_act, act, sizeof(*k_act));

	return 0;
}

//FIXME: Move alloca macros to arch/arm/kernel/signal.c

#define __process_alloca_with_align(__tsk, __ptr, __align) ({  \
       struct thread_info *__thrd = task_thread_info(__tsk);   \
       u32 __sp = __thrd->user.psp;                            \
       __sp -= sizeof(__typeof__(*(__ptr)));                   \
       __sp = align(__sp, __align);                            \
       __thrd->user.psp = __sp;                                \
       __ptr = (__typeof__(__ptr))__sp; })

#define __process_alloca(__tsk, __ptr) __process_alloca_with_align(__tsk, __ptr, 1)

static void __send_signal(struct task_struct *tsk, int sig,
			struct sigaction *sa, union sigval value)
{
	siginfo_t *siginfo = NULL;
	struct cpu_user_context *sigctx;

	if (sa->sa_flags & SA_SIGINFO) {
		__process_alloca(tsk, siginfo);
		siginfo->si_signo = sig;
		siginfo->si_value = value;
		siginfo->si_pid = current->tgid;
	}

	__process_alloca_with_align(tsk, sigctx, 8);
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

	if (tsk == current)
		current_thread_info()->bypass_update_r0 = 1;
}

static int send_signal(struct task_struct *tsk, int sig, union sigval value)
{
	struct sigaction *act;

	if (!tsk->sighand)
		return 0;
	act = &tsk->sighand->action[sig];
	if (!act->sa_handler)
		return 0;
	tsk->sigpending = sig;
	__send_signal(tsk, sig, act, value);

	return 0;
}

static int do_kill(int pid, int sig, union sigval value)
{
	struct task_struct *tsk;

	tsk = get_task_by_pid(pid);
	if (!tsk) {
		//errno = ESRCH;
		return -1;
	}

	return send_signal(tsk, sig, value);
}

SYSCALL_DEFINE(sigqueue,
	pid_t		pid,
	int		sig,
	union sigval	value)
{
	return do_kill(pid, sig, value);
}

SYSCALL_DEFINE(kill,
	pid_t		pid,
	int		sig)
{
	return do_kill(pid, sig, (union sigval){0});
}

SYSCALL_DEFINE(sigreturn, void)
{
	struct sigaction *act;
	int off;

	if (current->sigpending == -1) {
		pr_warn("Wants to return from signal, but no signal raised");
		return 0;
	}

	act = &current->sighand->action[current->sigpending];
	off = sizeof(struct cpu_user_context);
	if (act->sa_flags & SA_SIGINFO)
		off += align_next(sizeof(siginfo_t), 8);
	current_thread_info()->user.psp += off;
	current->sigpending = -1;

	return 0;
}

void do_sigevent(struct task_struct *tsk, struct sigevent *sigev)
{
	if (sigev->sigev_notify != SIGEV_SIGNAL)
		return;
	send_signal(tsk, sigev->sigev_signo, sigev->sigev_value);
}
