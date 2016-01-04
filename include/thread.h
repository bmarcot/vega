#ifndef THREAD_H
#define THREAD_H


#include <stdbool.h>

#include "linux/types.h"
#include "linux/list.h"

#include "pthread.h"

#define INTR_STACK_ORDER 10 // 0x400 = 1kB
#define INTR_STACK_SIZE (1 << INTR_STACK_ORDER)

struct thread_info {
	/* v7m data, could be moved to arch-dependent file */
	void *ti_msp;   // +0
	void *ti_psp;   // +4             //uint32_t
	u32 ti_priv;    // +8

	/* thread description data */
	void *(*ti_entry)(void *); //FIXME: not needed... unless we implement thread_reset()
	void *ti_args;             //FIXME: not needed... unless we implement thread_reset()
	char ti_name[16];          /* unused - no name for threads, just ids */
	//enum thread_privilege ti_priv;
	int ti_prio;               /* unused */
	pthread_t ti_id;

	/* shared by scheduler runq and mutex waitq */
	struct list_head ti_list; // global list of threads
	struct list_head ti_q;    // shared by sched runq, mutex waitq, thread joinq

	/* thread join's data */
	bool ti_joinable; // init false
	void *ti_retval;
	struct list_head ti_joinq;
	struct list_head *ti_joining;

	/* http://www.domaigne.com/blog/computing/joinable-and-detached-threads/ */
	bool ti_detached; // init false

	/* /\* local-storage *\/ */
	/* struct list_head *ti_lsq; // local-storage queue */
};

enum thread_privilege {
	THREAD_PRIV_SUPERVISOR = 0,
	THREAD_PRIV_USER       = 1
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

struct _intr_stackframe {
	u32 is_gprs[8];	/* task-context r4 to r11, zero-filled */
	u32 is_lr;      /* loaded with EXC_RETURN value */
};

// '_' prefix because 'hidden' structure
//struct basic_stackframe { // lazy pushed
struct _thrd_stackframe {
	u32 ts_gprs[5];	 /* thread-context r0 to r3, r12, zero-filled */
	u32 ts_lr;	 /* this should be loaded with _exit() */
	u32 ts_ret_addr; /* entry-point to the thread, usually main() */
	u32 ts_xpsr;	 /* forced to Thumb Mode */
};

/* forward declarations */

void switch_to(struct thread_info *, struct thread_info *);
struct thread_info *thread_create(void *(*)(void *), void *);
void thread_yield(void);

// move to assembler.h (no because it's not inlined) / entry.S ?
struct thread_info *current_thread_info(void);

#define CURRENT_THREAD_INFO(var)				\
	struct thread_info *var = current_thread_info();

#endif /* !THREAD_H */
