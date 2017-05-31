/*
 * include/kernel/thread.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_THREAD_H
#define _KERNEL_THREAD_H

#include <sys/types.h>

#include <kernel/kernel.h>
#include <kernel/types.h>

#include <arch/thread_info.h>

#include "linux/list.h"

#define INTR_STACK_ORDER  9  /* 512 Bytes */
#define INTR_STACK_SIZE   (1 << INTR_STACK_ORDER)

enum thread_privilege {
	THREAD_PRIV_SUPERVISOR = 0,
	THREAD_PRIV_USER       = 1
};

enum thread_state {
	/* Thread structure allocated but not enqueued in the system scheduler. */
	THREAD_STATE_NEW,

	/* Ready to run in the system scheduler. */
	THREAD_STATE_READY,

	/* Running by the system scheduler. */
	THREAD_STATE_RUNNING,

	/* The thread has normally exited or has called Pthread_exit to exit. Its
	 * resources have not been freed and will be freed if it is detached or
	 * joined.    */
	THREAD_STATE_TERMINATED,

	/* Waiting for a mutex or resource. */
	THREAD_STATE_BLOCKED
};

/*
 * This stackframe is built to handle the first scheduling of a task on
 * the CPU. Running a task for the first time is achieved in two stages.
 *
 * Stage 1:
 *   After switch_to() has switched the interrupt stacks for current and
 *   next tasks the function will restore the kernel-context for the next
 *   thread. That context is the non-scratch registers r4 to r11. At this
 *   point we don't care about the values in these registers: they are
 *   not used after that point during in that interrupt, and the next
 *   interrupt context will reinitilized them should it need to.
 *
 * Stage 2:
 *   After restoring the non-scratch registers we return from switch_to()
 *   and the value loaded in LR in stage1 is the address of task_kickstart().
 *   This function is a trampoline that emulate a return 'from interrupt
 *   sequence' by restoring the non-scratch registers for the tasks as
 *   well as triggering a switch from Handler_Mode to Thread_Mode in the
 *   CPU. We initialize the task non-scratch registers to 0.
 */

#include <kernel/fs.h>

struct task_struct {
	struct thread_info *info;

	int                ti_priority;
	int                ti_id;
	int                ti_state;
	int                ti_stacksize; /* thread stack's size */

	struct list_head   ti_list; /* global list of threads */
	struct list_head   ti_q;    /* sched runq, mutex waitq, thread joinq */

	/* http://www.domaigne.com/blog/computing/joinable-and-detached-threads/ */
	void               *ti_retval;
	int                ti_detached;
	int                ti_joinable;
	struct thread_info *ti_joining;

	/* Pointer to mutually exclusive data: the mutex the thread is blocking
	 * on, the exit value when thread is not yet joined, etc. */
	void               *ti_private;

	/* old task_info struct */
	pid_t            pid;
	unsigned long    filemap;
	struct file      *filetable[FILE_MAX];
};

#define THREAD_SIZE 512

union thread_union {
	struct thread_info thread_info;
	unsigned int       stack[THREAD_SIZE / sizeof(int)];
};

/* forward declarations */

void switch_to(struct thread_info *, struct thread_info *);
void thread_restore(struct thread_info *); //FIXME: rename to switch_to_restore_only ? meh..

typedef void *(*start_routine)(void *);

struct thread_info *thread_create(void *(*)(void *), void *,
				enum thread_privilege, size_t);
int thread_yield(void);
int thread_self(void);
void thread_exit(void *);
int thread_set_priority(struct thread_info *thread, int priority);
int thread_detach(pthread_t thread);
struct thread_info *thread_clone(struct thread_info *other, void *arg);

#define CURRENT_THREAD_INFO(var) \
	struct thread_info *var = current_thread_info();

#define THREAD_INFO(addr)					\
	({ (struct thread_info *)align((unsigned long)addr,	\
					INTR_STACK_SIZE); })

#define THREAD_CANARY0 0xee48a608
#define THREAD_CANARY1 0x840dc3bc

#ifdef CONFIG_KERNEL_STACK_CHECKING
#  define KERNEL_STACK_CHECKING  ({					\
	__auto_type cur_thread = current_thread_info();			\
	if ((cur_thread->ti_canary[0] != THREAD_CANARY0)		\
	    || (cur_thread->ti_canary[1] != THREAD_CANARY1)) {		\
		printk("\nkernel panic: Overflow in kernel stack\n");	\
		printk("  0  %08x  %08x\n", THREAD_CANARY0, cur_thread->ti_canary[0]); \
		printk("  1  %08x  %08x\n", THREAD_CANARY1, cur_thread->ti_canary[1]); \
		for (;;)						\
			;						\
	} })
#else
#  define KERNEL_STACK_CHECKING
#endif

#endif /* !_KERNEL_THREAD_H */
