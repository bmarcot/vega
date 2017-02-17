/*
 * kernel/thread.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <ucontext.h>

#include <arch/v7m-helper.h>

#include <kernel/errno-base.h>
#include <kernel/kernel.h>
#include <kernel/page.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/types.h>

#include "utils.h"
#include "platform.h"

extern struct task_info top_task;

static struct kernel_context_regs *alloc_interrupt_stack(void)
{
	char *memp;
	struct kernel_context_regs *kcr;

	memp = alloc_pages(size_to_page_order(INTR_STACK_SIZE));
	if (memp == NULL)
		return NULL;
	kcr = (struct kernel_context_regs *)
		(memp + INTR_STACK_SIZE - sizeof(struct kernel_context_regs));
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	memset(kcr->r4_r7, 0, 4 * sizeof(u32));
	memset(kcr->r8_r11, 0, 4 * sizeof(u32));
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	memset(kcr->r4_r12, 0, 9 * sizeof(u32));
#endif
	kcr->lr = V7M_EXC_RETURN_THREAD_PROCESS;

	return kcr;
}

/* new thread's LR loaded with pthread_exit address */
void pthread_exit(void *retval);

//XXX: Detecting a stack-overflow on v7M: #memf && (SP_Process == MMFAR)
static struct thread_context_regs *alloc_thread_stack(
	void *(*start_routine)(void *),	void *arg, size_t stacksize)
{
	char *memp;
	struct thread_context_regs *tcr;

	memp = alloc_pages(size_to_page_order(stacksize));
	if (!memp)
		return NULL;
	tcr = (struct thread_context_regs*)
		(memp + stacksize - sizeof(struct thread_context_regs));
	tcr->r0_r3__r12[0] = (u32)arg;
	memset(&tcr->r0_r3__r12[1], 0, 4 * sizeof(u32));
	tcr->lr = (u32)pthread_exit; //FIXME: Should libc be dynamically loaded?
	tcr->ret_addr = (u32)v7m_clear_thumb_bit(start_routine);
	tcr->xpsr = xPSR_T_Msk;

	return tcr;
}

struct thread_info *thread_create(void *(*start_routine)(void *), void *arg,
				enum thread_privilege priv, size_t stacksize)
{
	struct thread_info *thread;
	struct kernel_context_regs *kcr;
	struct thread_context_regs *tcr;
	static int thread_count = 0;

	kcr = alloc_interrupt_stack();
	if (kcr == NULL)
		return NULL;
	tcr = alloc_thread_stack(start_routine,	arg, stacksize);
	thread = THREAD_INFO(kcr);
	if (tcr == NULL) {
		free(thread);
		return NULL;
	}
	thread->ti_mach.mi_psp = (u32)tcr;
	thread->ti_mach.mi_msp = (u32)kcr;
	thread->ti_mach.mi_priv = priv;
	thread->ti_stacksize = stacksize;
	thread->ti_id = thread_count++;
	thread->ti_joinable = false;
	thread->ti_joining = NULL;
	thread->ti_detached = false;
	thread->ti_priority = PRI_MIN;
	thread->ti_state = THREAD_STATE_NEW;
#ifdef CONFIG_KERNEL_STACK_CHECKING
	thread->ti_canary[0] = THREAD_CANARY0;
	thread->ti_canary[1] = THREAD_CANARY1;
#endif
	list_add(&thread->ti_list, &top_task.thread_head);

	return thread;
}

int thread_yield(void)
{
#ifdef DEBUG
	CURRENT_THREAD_INFO(thread);
	printk("thread: id=%d is yielding\n", thread->ti_id);
#endif /* DEBUG */

	//FIXME: elect iff there is a higher-priority thread ready to run
	CURRENT_THREAD_INFO(curr_thread);
	sched_enqueue(curr_thread);

	return sched_elect(SCHED_OPT_NONE);
}

int thread_self(void)
{
	CURRENT_THREAD_INFO(curr_thread);

	return curr_thread->ti_id;
}

void thread_exit(void *retval)
{
	CURRENT_THREAD_INFO(curr_thread);

#ifdef DEBUG
	printk("thread: id=%d is exiting with retval=%d\n", current->ti_id, (int) retval);
#endif

	/* free thread stack memory */
	free_pages(align(curr_thread->ti_mach.mi_psp, curr_thread->ti_stacksize),
		size_to_page_order(curr_thread->ti_stacksize));

	if (curr_thread->ti_detached == false) {
		curr_thread->ti_retval = retval;
		if (curr_thread->ti_joining)
			sched_enqueue(curr_thread->ti_joining);
		else
			curr_thread->ti_joinable = true;
	} else {
		/* We are freeing the stack we are running on, no kernel preemption
		 * is allowed until we call sched_elect().  */
		free_pages((unsigned long)curr_thread,
			size_to_page_order(INTR_STACK_SIZE));
	}

	sched_elect(SCHED_OPT_RESTORE_ONLY);
}

int thread_set_priority(struct thread_info *thread, int priority)
{
	/* priority change is effective on next scheduling */
	thread->ti_priority = priority;

	return 0;
}

static struct thread_info *find_thread_by_id(int id)
{
	struct thread_info *tp;

	list_for_each_entry(tp, &top_task.thread_head, ti_list) {
		if (tp->ti_id == id)
			return tp;
	}

	return NULL;
}

int thread_join(pthread_t thread, void **retval)
{
	struct thread_info *other;

	other = find_thread_by_id(thread);
	if (other == NULL)
		return -ESRCH;  /* No thread with the ID thread could be found. */
	if (other->ti_detached == true)
		return -EINVAL;  /* thread is not a joinable thread. */

	/* the other thread is not yet joinable, the current thread blocks */
	if (other->ti_joinable == false) {
		CURRENT_THREAD_INFO(curr_thread);
		if (other->ti_joining)
			return -EINVAL;  /* Another thread is already waiting to
					    join with this thread. */
		other->ti_joining = curr_thread;
		sched_elect(SCHED_OPT_NONE);
	}
	*retval = other->ti_retval;

	//XXX: free other's resources, interrupt stack

	return 0;
}

int thread_detach(pthread_t thread)
{
	struct thread_info *thread_info;

	thread_info = find_thread_by_id(thread);
	thread_info->ti_detached = true;

	return 0;
}

/* pthread interface */

int sys_pthread_yield(void)
{
	return thread_yield();
}

pthread_t sys_pthread_self(void)
{
	return (pthread_t) thread_self();
}

void sys_pthread_exit(void *retval)
{
	thread_exit(retval);
}

int sys_pthread_detach(pthread_t thread)
{
	return thread_detach(thread);
}

int sys_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
	struct rlimit stacklimit;
	size_t stacksize;

	/* get the thread default stack size */
	sys_getrlimit(RLIMIT_STACK, &stacklimit);
	if (attr)
		stacksize = min(attr->stacksize, stacklimit.rlim_max);
	else
		stacksize = stacklimit.rlim_cur;

	//FIXME: Check start_routine's address belongs to process' address-space
	struct thread_info *thread_info =
		thread_create(start_routine, arg, THREAD_PRIV_USER, stacksize);
	if (thread_info == NULL)
		return EAGAIN; /* insufficient resources to create another thread */
	*thread = (pthread_t)thread_info->ti_id;
	sched_enqueue(thread_info);

	return 0;
}

int sys_pthread_join(pthread_t thread, void **retval)
{
	return thread_join(thread, retval);
}
