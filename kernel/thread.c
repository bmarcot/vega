/*
 * kernel/thread.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <pthread.h>  /* include pthread.h because lr loaded with pthread_exit() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <ucontext.h>

#include <kernel/errno-base.h>
#include <kernel/page.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/types.h>
#include <kernel/v7m-helper.h>

#include "utils.h"
#include "kernel.h"
#include "cmsis/arm/ARMCM4.h"

static LIST_HEAD(thread_list); /* global linked-list of all threads */

static struct kernel_context_regs *build_intr_stack(void)
{
	void *memp;
	struct kernel_context_regs *kcr;

	/* We don't need a huge stack for interrupt handling, however we need the page
	   to be aligned on a known value to retrieve the Thread Control Block that
	   is stored at the bottom of the physical page.    */
	memp = alloc_pages(size_to_page_order(INTR_STACK_SIZE));
	if (memp == NULL)
		return NULL;
	kcr = (struct kernel_context_regs *)
		((unsigned long)memp + INTR_STACK_SIZE - sizeof(struct kernel_context_regs));
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	memset(kcr->r4_r7, 0, 4 * sizeof(u32));
	memset(kcr->r8_r11, 0, 4 * sizeof(u32));
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	memset(kcr->r4_r12, 0, 9 * sizeof(u32));
#endif
	kcr->lr = V7M_EXC_RETURN_THREAD_PROCESS;

	return kcr;
}

// pass user-supplied stack size - configure task stack size
// note: detect stack overflow -> #memf && (SP_Process == MMFAR)
static struct thread_context_regs *build_thrd_stack(void *(*start_routine)(void *),
						void *arg, size_t stacksize)
{
	void *memp;
	struct thread_context_regs *tcr;

	memp = alloc_pages(size_to_page_order(stacksize));
	if (!memp)
		return NULL;
	tcr = (struct thread_context_regs*)
		((unsigned long)memp + stacksize - sizeof(struct thread_context_regs));
	tcr->r0_r3__r12[0] = (u32)arg;
	memset(&tcr->r0_r3__r12[1], 0, 4 * sizeof(u32));

	/* Calls pthread_exit() on return keyword. This might need to be fixed at
	   runtime in the future.    */
	tcr->lr = (u32)pthread_exit;
	tcr->ret_addr = (u32)v7m_clear_thumb_bit(start_routine);
	tcr->xpsr = xPSR_T_Msk;

	return tcr;
}

struct thread_info *thread_create(void *(*start_routine)(void *), void *arg,
				enum thread_privilege priv, size_t stacksize)
{
	struct thread_info *thread;
	struct kernel_context_regs *kcr;
	static int thread_count = 0;

	kcr = build_intr_stack();
	if (!kcr)
		return NULL;
	thread = (struct thread_info *)align((unsigned long)kcr, INTR_STACK_SIZE);
	thread->ti_mach.mi_psp = (u32)build_thrd_stack(start_routine, arg, stacksize);
	if (!thread->ti_mach.mi_psp) {
		//FIXME: free(kcr);
		return NULL;
	}
	thread->ti_mach.mi_msp = (u32)kcr;
	thread->ti_mach.mi_priv = priv;
	thread->ti_stacksize = stacksize;
	thread->ti_id = thread_count++;
	thread->ti_joinable = false;
	thread->ti_joining = NULL;
	thread->ti_detached = false;
	thread->ti_priority = PRI_MIN;  /* new threads are assigned the lowest priority */
	thread->ti_state = THREAD_STATE_NEW;
	INIT_LIST_HEAD(&thread->ti_sigactions);
#ifdef CONFIG_KERNEL_STACK_CHECKING
	thread->ti_canary[0] = THREAD_CANARY0;
	thread->ti_canary[1] = THREAD_CANARY1;
#endif
	list_add(&thread->ti_list, &thread_list);

	return thread;
}

int thread_yield(void)
{
#ifdef DEBUG
	CURRENT_THREAD_INFO(thread);
	printk("thread: id=%d is yielding\n", thread->ti_id);
#endif /* DEBUG */

	//FIXME: elect iff there is a higher-priority thread ready to run
	CURRENT_THREAD_INFO(current);
	sched_enqueue(current);

	return sched_elect(SCHED_OPT_NONE);
}

int thread_self(void)
{
	CURRENT_THREAD_INFO(thread);

	return thread->ti_id;
}

void thread_exit(void *retval)
{
	CURRENT_THREAD_INFO(current);

#ifdef DEBUG
	printk("thread: id=%d is exiting with retval=%d\n", current->ti_id, (int) retval);
#endif

	/* free thread stack memory */
	free_pages(align(current->ti_mach.mi_psp, current->ti_stacksize),
		size_to_page_order(current->ti_stacksize));

	if (current->ti_detached == false) {
		current->ti_retval = retval;
		if (current->ti_joining)
			sched_enqueue(current->ti_joining);
		else
			current->ti_joinable = true;
	} else {
		/* We are freeing the stack we are running on, no kernel preemption
		 * is allowed until we call sched_elect().  */
		free_pages((unsigned long)current,
			size_to_page_order(INTR_STACK_SIZE));
	}

	sched_elect(SCHED_OPT_RESTORE_ONLY);
}

int thread_set_priority(struct thread_info *thread, int priority)
{
	//FIXME: Must be called before the sched_enqueue!
	//if (thread->state != THREAD_STATE_NEW)
	// return -1;
	thread->ti_priority = priority;

	return 0;
}

static struct thread_info *find_thread_by_id(int id)
{
	struct thread_info *tp;

	list_for_each_entry(tp, &thread_list, ti_list) {
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
		CURRENT_THREAD_INFO(current);
		if (other->ti_joining)
			return -EINVAL;  /* Another thread is already waiting to
					    join with this thread. */
		other->ti_joining = current;
		sched_elect(SCHED_OPT_NONE);
	}
	*retval = other->ti_retval;

	//XXX: free other's resources, interrupt stack

	return 0;
}

int thread_detach(pthread_t thread)
{
	struct thread_info *tip;

	tip = find_thread_by_id(thread);
	tip->ti_detached = true;

	return 0;
}

/* pthread interface */

static ucontext_t main_context, pthread_context;
static unsigned int ctx_stack[256];
static int create_ret_code;

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

static void pthread_create_2(/* __user */ pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
	struct thread_info *thread_info;
	struct rlimit stacklimit;
	size_t stacksize;

	/* get the thread's stack size */
	sys_getrlimit(RLIMIT_STACK, &stacklimit);
	if (attr)
		stacksize = min(attr->stacksize, stacklimit.rlim_max);
	else
		stacksize = stacklimit.rlim_cur;

	/* FIXME: We must check all addresses of user-supplied pointers, they must belong
	   to this process user-space.    */
	thread_info = thread_create(start_routine, arg, THREAD_PRIV_USER, stacksize);
	if (!thread_info) {
		create_ret_code = -1;
		return;
	}
	*thread = (pthread_t)thread_info->ti_id;

	sched_enqueue(thread_info);
}

int sys_pthread_create(/* __user */ pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
	/* link the current context to the print context */
	pthread_context.uc_link = &main_context;
	pthread_context.uc_stack.ss_sp = &ctx_stack[256];

	/* pass the arguments to the new context, and swap */
	makecontext(&pthread_context, pthread_create_2, 4, thread, attr, start_routine,
		arg);
	swapcontext(&main_context, &pthread_context);

	return create_ret_code;
}

void sys_pthread_join(pthread_t thread, void **retval)
{
	thread_join(thread, retval);
}
