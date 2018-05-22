/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/sched.h>
#include <kernel/sched/signal.h>
#include <kernel/signal_types.h>
#include <kernel/string.h>

#include <uapi/kernel/signal.h>

#include <asm/bitsperlong.h>

/*
 * Primitives to manipulate sigset_t
 */

static inline void sigaddset(sigset_t *set, int sig)
{
	sig--;
	if (_NSIG_WORDS == 1)
		set->sig[0] |= (1ul << sig);
	else
		set->sig[sig / _NSIG_BPW] |= (1UL << (sig % _NSIG_BPW));
}

static inline void sigdelset(sigset_t *set, int sig)
{
	sig--;
	if (_NSIG_WORDS == 1)
		set->sig[0] &= ~(1ul << sig);
	else
		set->sig[sig / _NSIG_BPW] &= ~(1UL << (sig % _NSIG_BPW));
}

static inline int sigismember(sigset_t *set, int sig)
{
	sig--;
	if (_NSIG_WORDS == 1)
		return 1ul & (set->sig[0] >> sig);
	else
		return 1ul & (set->sig[sig / _NSIG_BPW] >> (sig % _NSIG_BPW));
}

static inline int sigisemptyset(sigset_t *set)
{
	switch (_NSIG_WORDS) {
	case 4:
		return (set->sig[3] | set->sig[2] |
			set->sig[1] | set->sig[0]) == 0;
	case 2:
		return (set->sig[1] | set->sig[0]) == 0;
	case 1:
		return set->sig[0] == 0;
	default:
		/* BUILD_BUG(); */
		return 0;
	}
}

static inline void sigemptyset(sigset_t *set)
{
	switch (_NSIG_WORDS) {
	case 2: set->sig[1] = 0;
	case 1: set->sig[0] = 0;
		break;
	default:
		memset(set, 0, sizeof(sigset_t));
	}
}

static inline void sigfillset(sigset_t *set)
{
	switch (_NSIG_WORDS) {
	case 2: set->sig[1] = -1;
	case 1: set->sig[0] = -1;
		break;
	default:
		memset(set, -1, sizeof(sigset_t));
	}
}

#define _SIG_SET_BINOP(name, op)					\
static inline void name(sigset_t *r, const sigset_t *a, const sigset_t *b) \
{									\
	unsigned long a0, a1, a2, a3, b0, b1, b2, b3;			\
									\
	switch (_NSIG_WORDS) {						\
	case 4:								\
		a3 = a->sig[3]; a2 = a->sig[2];				\
		b3 = b->sig[3]; b2 = b->sig[2];				\
		r->sig[3] = op(a3, b3);					\
		r->sig[2] = op(a2, b2);					\
	case 2:								\
		a1 = a->sig[1]; b1 = b->sig[1];				\
		r->sig[1] = op(a1, b1);					\
	case 1:								\
		a0 = a->sig[0]; b0 = b->sig[0];				\
		r->sig[0] = op(a0, b0);					\
		break;							\
	default:							\
		BUILD_BUG();						\
	}								\
}

#define _sig_or(x,y)((x) | (y))
_SIG_SET_BINOP(sigorsets, _sig_or)

#define _sig_and(x,y)((x) & (y))
_SIG_SET_BINOP(sigandsets, _sig_and)

#define _sig_andn(x,y)((x) & ~(y))
_SIG_SET_BINOP(sigandnsets, _sig_andn)

#if SIGRTMIN > __BITS_PER_LONG
#define rt_sigmask(sig)	(1ull << ((sig) - 1))
#else
#define rt_sigmask(sig)	(1ul << ((sig) - 1))
#endif

#define siginmask(sig, mask) \
	((sig) < SIGRTMIN && (rt_sigmask(sig) & (mask)))

#define SIG_KERNEL_ONLY_MASK (\
	rt_sigmask(SIGKILL)   |  rt_sigmask(SIGSTOP))

#define SIG_KERNEL_STOP_MASK (\
	rt_sigmask(SIGSTOP)   |  rt_sigmask(SIGTSTP)   | \
	rt_sigmask(SIGTTIN)   |  rt_sigmask(SIGTTOU)   )

#define SIG_KERNEL_COREDUMP_MASK (\
	rt_sigmask(SIGQUIT)   |  rt_sigmask(SIGILL)    | \
	rt_sigmask(SIGTRAP)   |  rt_sigmask(SIGABRT)   | \
	rt_sigmask(SIGFPE)    |  rt_sigmask(SIGSEGV)   | \
	rt_sigmask(SIGBUS)    |  rt_sigmask(SIGSYS)    | \
	rt_sigmask(SIGXCPU)   |  rt_sigmask(SIGXFSZ)   )

#define SIG_KERNEL_IGNORE_MASK (\
	rt_sigmask(SIGCONT)   |  rt_sigmask(SIGCHLD)   | \
	rt_sigmask(SIGURG)    )

#define SIG_SPECIFIC_SICODES_MASK (\
	rt_sigmask(SIGILL)    |  rt_sigmask(SIGFPE)    | \
	rt_sigmask(SIGSEGV)   |  rt_sigmask(SIGBUS)    | \
	rt_sigmask(SIGTRAP)   |  rt_sigmask(SIGCHLD)   | \
	rt_sigmask(SIGPOLL)   |  rt_sigmask(SIGSYS)    )

#define sig_kernel_only(sig)		siginmask(sig, SIG_KERNEL_ONLY_MASK)
#define sig_kernel_coredump(sig)	siginmask(sig, SIG_KERNEL_COREDUMP_MASK)
#define sig_kernel_ignore(sig)		siginmask(sig, SIG_KERNEL_IGNORE_MASK)
#define sig_kernel_stop(sig)		siginmask(sig, SIG_KERNEL_STOP_MASK)
#define sig_specific_sicodes(sig)	siginmask(sig, SIG_SPECIFIC_SICODES_MASK)

struct signal_struct *alloc_signal_struct(void);
void put_signal_struct(struct signal_struct *sig);
struct sighand_struct *copy_sighand_struct(struct task_struct *tsk);
void put_sighand_struct(struct sighand_struct *sig);
int send_signal_info(int sig, struct sigqueue *info, struct task_struct *tsk);
int send_rt_signal(struct task_struct *tsk, int sig, int value);
void purge_pending_signals(struct task_struct *tsk);
void do_signal(void);
int signal_init(void);

extern void __do_signal(int signo, struct sigqueue *sig);

static inline int signal_pending(struct task_struct *p)
{
	return test_tsk_thread_flag(p, TIF_SIGPENDING);
}

static inline void init_sigpending(struct sigpending *sig)
{
	sigemptyset(&sig->signal);
	INIT_LIST_HEAD(&sig->list);
}

static inline struct sighand_struct *task_sighand(struct task_struct *tsk)
{
	return tsk->sighand;
}

#define sig_ignore(t, signr)						\
	(((t)->sighand->action[(signr) - 1].sa_handler == SIG_IGN) ||	\
	 (siginmask(signr, SIG_KERNEL_IGNORE_MASK) &&			\
	  (t)->sighand->action[(signr) - 1].sa_handler == SIG_DFL))

#endif /* !_KERNEL_SIGNAL_H */
