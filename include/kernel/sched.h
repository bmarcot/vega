/*
 * include/kernel/sched.h
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#ifndef _KERNEL_SCHED_H
#define _KERNEL_SCHED_H

#include <kernel/compiler.h>
#include <kernel/signal_types.h>
#include <kernel/thread_info.h>
#include <kernel/types.h>

#include <uapi/kernel/sched.h>

/* 0 <= PRI_MAX <= PRI_MIN */
#define PRI_MAX    0
#define PRI_MIN    31

#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_STOPPED		8
#define EXIT_DEAD		16
#define EXIT_ZOMBIE		32
#define TASK_DEAD		64
#define TASK_NEW		2048

#define FILE_MAX 8

struct files_struct;
struct mm_struct;

struct task_struct {
#ifdef CONFIG_THREAD_INFO_IN_TASK
	struct thread_info	thread_info;
#endif
	void			*stack;
	int			state;
	int			flags;
	int			prio;
	struct list_head	rq;		/* runqueue */

	struct list_head	thread_group;
	struct task_struct	*group_leader;

	int			exit_code;
	int			exit_signal;
	pid_t			pid;		/* thread id */
	pid_t			tgid;		/* thread-group (process) id */

	struct list_head	list;		/* global list of tasks -- remove? */
	struct task_struct	*parent;

	/* signal handlers */
	struct signal_struct	*signal;
	struct sighand_struct	*sighand;
	sigset_t		blocked;
	struct sigpending	pending;

	/* memory mappings */
	struct mm_struct	*mm;

	/* open file information */
	struct files_struct	*files;
};

int set_task_state(struct task_struct *tsk, int state);

#define set_current_state(state_value)			\
	do {						\
		set_task_state(current, (state_value));	\
	} while (0)

#ifdef CONFIG_THREAD_INFO_IN_TASK
static inline struct thread_info *task_thread_info(struct task_struct *tsk)
{
	return &tsk->thread_info;
}
#else
#define task_thread_info(tsk) ((struct thread_info *)(tsk)->stack)
#endif

int sched_init(void);
int schedule(void);
int wake_up_process(struct task_struct *tsk);

/* task.c */
int init_task(struct task_struct *task, int flags);
int release_task_pids(struct task_struct *task);
void put_task_struct(struct task_struct *tsk);
void release_task(struct task_struct *tsk);
struct task_struct *get_task_by_pid(pid_t pid);
struct list_head *get_all_tasks(void); /* will die.. */
void add_task(struct task_struct *tsk); /* will die.. */
void reserve_pid(pid_t pid); /* will die.. */

/* fork.c */
struct task_struct *
do_clone(unsigned long flags, void *child_stack, struct pt_regs *regs);

/* asm/thread.c */
void arch_thread_setup(struct task_struct *tsk, unsigned long flags,
		void *child_stack, struct pt_regs *regs);

/* exit.c */
void do_exit(int exit_code);

/* init.c */
struct task_struct *alloc_init_task(void);

static inline int thread_group_leader(struct task_struct *tsk)
{
	return tsk->exit_signal >= 0;
}

/*
 * Set thread flags in other task's structures.
 * See asm/thread_info.h for TIF_xxxx flags available:
 */

static inline void set_tsk_thread_flag(struct task_struct *tsk, int flag)
{
	set_ti_thread_flag(task_thread_info(tsk), flag);
}

static inline void clear_tsk_thread_flag(struct task_struct *tsk, int flag)
{
	clear_ti_thread_flag(task_thread_info(tsk), flag);
}

static inline int test_tsk_thread_flag(struct task_struct *tsk, int flag)
{
	return test_ti_thread_flag(task_thread_info(tsk), flag);
}

static inline void set_tsk_need_resched(struct task_struct *tsk)
{
	set_tsk_thread_flag(tsk,TIF_NEED_RESCHED);
}

static inline void clear_tsk_need_resched(struct task_struct *tsk)
{
	clear_tsk_thread_flag(tsk,TIF_NEED_RESCHED);
}

static inline int test_tsk_need_resched(struct task_struct *tsk)
{
	return unlikely(test_tsk_thread_flag(tsk,TIF_NEED_RESCHED));
}

#endif /* !_KERNEL_SCHED_H */
