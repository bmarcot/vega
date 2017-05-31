/*
 * kernel/thread.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <ucontext.h>

#include <vega/sys/resource.h>

#include <arch/thread_info.h>
#include <arch/v7m-helper.h>

#include <kernel/errno-base.h>
#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/types.h>

#include "utils.h"
#include "platform.h"

static struct preserved_context *alloc_interrupt_stack(void)
{
	char *memp = alloc_pages(size_to_page_order(INTR_STACK_SIZE));
	if (memp == NULL)
		return NULL;

	struct preserved_context *kcr = (struct preserved_context *)
		(memp + INTR_STACK_SIZE - sizeof(struct preserved_context));
	memset(kcr->r4_r12, 0, 9 * sizeof(__u32));
	kcr->lr = V7M_EXC_RETURN_THREAD_PROCESS;

	return kcr;
}

/* new thread's LR loaded with pthread_exit address */
void pthread_exit(void *retval);

//XXX: Detecting a stack-overflow on v7M: #memf && (SP_Process == MMFAR)
static struct cpu_saved_context *alloc_thread_stack(
	void *(*start_routine)(void *),	void *arg, size_t stacksize)
{
	char *memp = alloc_pages(size_to_page_order(stacksize));
	if (!memp)
		return NULL;

	struct cpu_saved_context *tcr = (struct cpu_saved_context*)
		(memp + stacksize - sizeof(struct cpu_saved_context));
	memset(&tcr->r0_r3__r12[1], 0, 4 * sizeof(__u32));
	tcr->r0_r3__r12[0] = (__u32)arg;
	tcr->lr = (__u32)pthread_exit; //FIXME: Should libc be dynamically loaded?
	tcr->ret_addr = (__u32)v7m_clear_thumb_bit(start_routine);
	tcr->xpsr = xPSR_T_Msk;

	return tcr;
}

struct thread_info *thread_create(void *(*start_routine)(void *), void *arg,
				enum thread_privilege priv, size_t stacksize,
				struct task_info *task)
{
	struct thread_info *thread;
	struct preserved_context *kcr;
	struct cpu_saved_context *tcr;
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
	thread->thread_ctx.ctx = tcr;
	thread->kernel_ctx.ctx = kcr;
	thread->priv = priv;

	/* task_struct struct is still located in the kernel stack'page,
	 * right after the thread_info struct. */
	thread->ti_struct = (struct task_struct *)thread->ti_stacktop;

	thread->ti_struct->info = thread;
	thread->ti_struct->ti_stacksize = stacksize;
	thread->ti_struct->ti_id = thread_count++;
	thread->ti_struct->ti_task = task;
	thread->ti_struct->ti_joinable = false;
	thread->ti_struct->ti_joining = NULL;
	thread->ti_struct->ti_detached = false;
	thread->ti_struct->ti_priority = PRI_MIN;
	thread->ti_struct->ti_state = THREAD_STATE_NEW;
#ifdef CONFIG_KERNEL_STACK_CHECKING
	thread->ti_canary[0] = THREAD_CANARY0;
	thread->ti_canary[1] = THREAD_CANARY1;
#endif
	list_add(&thread->ti_struct->ti_list, &task->thread_head);

	return thread;
}

static inline struct thread_info *init_thread_info(struct thread_info *thread)
{
	static int next_tid = 3000;

	thread->ti_struct->ti_stacksize = 0;
	thread->ti_struct->ti_id = next_tid++;
	thread->ti_struct->ti_task = NULL;
	thread->ti_struct->ti_joinable = false;
	thread->ti_struct->ti_joining = NULL;
	thread->ti_struct->ti_detached = false;
	thread->ti_struct->ti_priority = PRI_MIN;
	thread->ti_struct->ti_state = THREAD_STATE_NEW;
#ifdef CONFIG_KERNEL_STACK_CHECKING
	thread->ti_canary[0] = THREAD_CANARY0;
	thread->ti_canary[1] = THREAD_CANARY1;
#endif

	return thread;
}

struct thread_info *thread_clone(struct thread_info *other, void *arg,
				struct task_info *task /* will die.. */)
{
	struct thread_info *new;
	struct preserved_context *kcr;
	struct cpu_saved_context *tcr;

	kcr = alloc_interrupt_stack();
	if (kcr == NULL)
		return NULL;

	struct cpu_saved_context *other_tcr = other->thread_ctx.ctx;
	tcr = alloc_thread_stack((start_routine)other_tcr->ret_addr, arg,
				other->ti_struct->ti_stacksize);
	memcpy(tcr, other_tcr, sizeof(struct cpu_saved_context));
	tcr->r0_r3__r12[0] = (__u32)arg;

	new = THREAD_INFO(kcr);
	init_thread_info(new);
	if (tcr == NULL) {
		free(new);
		return NULL;
	}
	new->thread_ctx.ctx = tcr;
	new->kernel_ctx.ctx = kcr;
	new->priv = other->priv;

	/* see comment in thread_create() */
	new->ti_struct = (struct task_struct *)new->ti_stacktop;

	new->ti_struct->info = new;
	new->ti_struct->ti_stacksize = other->ti_struct->ti_stacksize;
	new->ti_struct->ti_task = task;
	list_add(&new->ti_struct->ti_list, &task->thread_head);

	return new;
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
	return current_thread_info()->ti_struct->ti_id;
}

void thread_exit(void *retval)
{
	CURRENT_THREAD_INFO(curr_thread);

#ifdef DEBUG
	printk("thread: id=%d is exiting with retval=%d\n", curr_thread->ti_struct->ti_id,
		(int) retval);
#endif

	/* free thread stack memory */
	free_pages(align(curr_thread->thread_ctx.sp, curr_thread->ti_struct->ti_stacksize),
		size_to_page_order(curr_thread->ti_struct->ti_stacksize));

	if (curr_thread->ti_struct->ti_detached == false) {
		curr_thread->ti_struct->ti_retval = retval;
		if (curr_thread->ti_struct->ti_joining)
			sched_enqueue(curr_thread->ti_struct->ti_joining);
		else
			curr_thread->ti_struct->ti_joinable = true;
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
	thread->ti_struct->ti_priority = priority;

	return 0;
}

static struct thread_info *find_thread_by_id(int id)
{
	struct task_struct *tp;
	CURRENT_TASK_INFO(curr_task);

	list_for_each_entry(tp, &curr_task->thread_head, ti_list) {
		if (tp->ti_id == id)
			return tp->info;
	}

	return NULL;
}

int thread_join(pthread_t thread, void **retval)
{
	struct thread_info *other;

	other = find_thread_by_id(thread);
	if (other == NULL)
		return -ESRCH;  /* No thread with the ID thread could be found. */
	if (other->ti_struct->ti_detached == true)
		return -EINVAL;  /* thread is not a joinable thread. */

	/* the other thread is not yet joinable, the current thread blocks */
	if (other->ti_struct->ti_joinable == false) {
		CURRENT_THREAD_INFO(curr_thread);
		if (other->ti_struct->ti_joining)
			return -EINVAL;  /* Another thread is already waiting to
					    join with this thread. */
		other->ti_struct->ti_joining = curr_thread;
		sched_elect(SCHED_OPT_NONE);
	}
	*retval = other->ti_struct->ti_retval;

	//XXX: free other's resources, interrupt stack

	return 0;
}

int thread_detach(pthread_t thread)
{
	struct thread_info *thread_info;

	thread_info = find_thread_by_id(thread);
	thread_info->ti_struct->ti_detached = true;

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
		stacksize = MIN(attr->stacksize, stacklimit.rlim_max);
	else
		stacksize = stacklimit.rlim_cur;

	//FIXME: Check start_routine's address belongs to process' address-space
	struct thread_info *thread_info =
		thread_create(start_routine, arg, THREAD_PRIV_USER, stacksize,
			current_task_info());
	if (thread_info == NULL)
		return EAGAIN; /* insufficient resources to create another thread */
	*thread = (pthread_t)thread_info->ti_struct->ti_id;
	sched_enqueue(thread_info);

	return 0;
}

int sys_pthread_join(pthread_t thread, void **retval)
{
	return thread_join(thread, retval);
}
