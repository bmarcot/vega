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
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/types.h>

#include "utils.h"
#include "platform.h"

/* static */ LIST_HEAD(thread_head);

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

static pid_t pid = 8000;

struct thread_info *thread_create(void *(*start_routine)(void *), void *arg,
				enum thread_privilege priv, size_t stacksize)
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
#ifndef CONFIG_THREAD_INFO_IN_TASK
	thread->task = (struct task_struct *)(thread + 1);
#endif

	struct task_struct *task = TASK_STRUCT(thread);
	task->pid = pid++;
	task->filemap = 0;
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;

#ifndef CONFIG_THREAD_INFO_IN_TASK
	task->thread_info = thread;
#endif
	task->ti_stacksize = stacksize;
	task->ti_id = thread_count++;
	task->ti_priority = PRI_MIN;
	task->ti_state = THREAD_STATE_NEW;
	list_add(&task->ti_list, &thread_head);

	return thread;
}

static inline struct thread_info *init_thread_info(struct thread_info *thread)
{
	static int next_tid = 3000;
	struct task_struct *task = TASK_STRUCT(thread);

	task->ti_stacksize = 0;
	task->ti_id = next_tid++;
	task->ti_priority = PRI_MIN;
	task->ti_state = THREAD_STATE_NEW;

	return thread;
}

struct thread_info *thread_clone(struct thread_info *other, void *arg)
{
	struct thread_info *new;
	struct preserved_context *kcr;
	struct cpu_saved_context *tcr;

	kcr = alloc_interrupt_stack();
	if (kcr == NULL)
		return NULL;

	struct task_struct *other_task = TASK_STRUCT(other);
	struct cpu_saved_context *other_tcr = other->thread_ctx.ctx;
	tcr = alloc_thread_stack((start_routine)other_tcr->ret_addr, arg,
				other_task->ti_stacksize);
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
#ifndef CONFIG_THREAD_INFO_IN_TASK
	new->task = (struct task_struct *)(new + 1);
#endif

	struct task_struct *task = TASK_STRUCT(new);
	task->pid = pid++;
	task->filemap = 0;
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;

#ifndef CONFIG_THREAD_INFO_IN_TASK
	task->thread_info = new;
#endif
	task->ti_stacksize = other_task->ti_stacksize;
	list_add(&task->ti_list, &thread_head);

	return new;
}

int thread_yield(void)
{
#ifdef DEBUG
	CURRENT_THREAD_INFO(thread);
	printk("thread: id=%d is yielding\n", thread->ti_id);
#endif /* DEBUG */

	//FIXME: elect iff there is a higher-priority thread ready to run
	sched_enqueue(get_current());

	return sched_elect(SCHED_OPT_NONE);
}

int thread_self(void)
{
	return get_current()->ti_id;
}

int thread_set_priority(struct thread_info *thread, int priority)
{
	/* priority change is effective on next scheduling */
	TASK_STRUCT(thread)->ti_priority = priority;

	return 0;
}

/* pthread interface */

int sys_pthread_yield(void)
{
	return thread_yield();
}
