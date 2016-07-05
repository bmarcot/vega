#ifndef THREAD_H
#define THREAD_H

#include "linux/types.h"
#include "linux/list.h"
#include "pthread.h"

#define INTR_STACK_ORDER 8    /* 0x100 = 256 Bytes */
#define INTR_STACK_SIZE (1 << INTR_STACK_ORDER)

/* machine-specific thread info on ARM */
struct mthread_info {
	u32 mi_msp;     /* +0 */
	u32 mi_psp;     /* +4 */
	u32 mi_priv;    /* +8 */
};

struct thread_info {
	/* machine-specific thread info */
	struct mthread_info ti_mach;

	/* thread description data */
	int ti_prio;               /* unused */
	int ti_id;

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

struct kernel_context_regs {
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	u32 r8_r11[4];
	//FIXME: restore r12 also? yes because kernel might use it! This is the
	//       kernel context (non-scratch), not the non-scratch for the user
	//       thread: they are implicitly restored by the epilogues for those
	//       used by kernel, or filled with 0 from there.
	//       u32 r8__r12[5];
	u32 r4_r7[4];
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	u32 r4_r11[8];  /* r4 to r11, zero-filled */
#endif
	u32 lr;         /* initially loaded with EXC_RETURN value */
};

struct thread_context_regs {
	u32 r0_r3__r12[5];  /* r0 to r3, r12; args or zero-filled */
	u32 lr;	            /* initially loaded with pthread_exit() */
	u32 ret_addr;       /* thread entry-point function */
	u32 xpsr;           /* forced to Thumb_Mode */
};

/* forward declarations */

void switch_to(struct thread_info *, struct thread_info *);
void thread_restore(struct thread_info *); //FIXME: rename to switch_to_restore_only ? meh..

struct thread_info *thread_create(void *(*)(void *), void *, enum thread_privilege, const pthread_attr_t *);
int thread_yield(void);
int thread_self(void);
void thread_exit(void *);

// move to assembler.h (no because it's not inlined) / entry.S ?
struct thread_info *current_thread_info(void);

#define CURRENT_THREAD_INFO(var)				\
	struct thread_info *var = current_thread_info();

#endif /* !THREAD_H */
