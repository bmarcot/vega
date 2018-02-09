/*
 * include/kernel/signal.h
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#ifndef _KERNEL_SIGNAL_H
#define _KERNEL_SIGNAL_H

#include <kernel/list.h>
#include <kernel/sched.h>
#include <kernel/sched/signal.h>
#include <kernel/signal_types.h>
#include <kernel/string.h>

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

struct signal_struct *alloc_signal_struct(struct task_struct *tsk);
struct sighand_struct *alloc_sighand_struct(struct task_struct *tsk);

int signal_pending(struct task_struct *tsk);
int send_signal_info(int sig, struct sigqueue *info, struct task_struct *tsk);
int send_rt_signal(struct task_struct *tsk, int sig, int value);
void do_signal(void);

extern void __do_signal(int signo, struct sigqueue *sig);

static inline void init_sigpending(struct sigpending *sig)
{
	sigemptyset(&sig->signal);
	INIT_LIST_HEAD(&sig->list);
}

static inline struct sighand_struct *task_sighand(struct task_struct *tsk)
{
	return tsk->sighand;
}

#endif /* !_KERNEL_SIGNAL_H */
