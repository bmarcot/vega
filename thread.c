#include <stdio.h>
#include <string.h>

#include "libc/stdlib.h"
#include "thread.h"
#include "pthread.h"
#include "mm.h"
#include "utils.h"
#include "sched-rr.h"
#include "arch-v7m.h"
#include "linux/types.h"
#include "kernel.h"
#include "cmsis/arm/ARMCM4.h"
#include "sys/resource.h"

static struct kernel_context_regs *build_intr_stack(void)
{
	void *memp;
	struct kernel_context_regs *kcr;

	/* We don't need a huge stack for interrupt handling, however we need the page
	   to be aligned on a known value to retrieve the Thread Control Block that
	   is stored at the bottom of the physical page.    */
	if ((memp = page_alloc(INTR_STACK_SIZE)) == NULL)
		return NULL;
	kcr = (void *)((u32) memp + INTR_STACK_SIZE - sizeof (struct kernel_context_regs));
#ifndef DEBUG
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	memset(kcr->r4_r7, 0, 4 * sizeof (u32));
	memset(kcr->r8_r11, 0, 4 * sizeof (u32));
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	memset(kcr->r4_r11, 0, 8 * sizeof (u32));
#endif
#else
	kcr->gprs[0] = 0xcafe0004;
	kcr->gprs[1] = 0xcafe0005;
	kcr->gprs[2] = 0xcafe0006;
	kcr->gprs[3] = 0xcafe0007;
	kcr->gprs[4] = 0xcafe0008;
	kcr->gprs[5] = 0xcafe0009;
	kcr->gprs[5] = 0xcafe0010;
	kcr->gprs[7] = 0xcafe0011;
#endif /* !DEBUG */
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

	if ((memp = page_alloc(stacksize)) == NULL)
		return NULL;
	tcr = (void *)((u32) memp + stacksize - sizeof(struct thread_context_regs));
	tcr->r0_r3__r12[0] = (u32) arg;
#ifndef DEBUG
	memset(&tcr->r0_r3__r12[1], 0, 4 * sizeof (u32));
#else
	printk("thread sp = %p\n", ts);
	tcr->gprs[1] = 0xcafe0001;
	tcr->gprs[2] = 0xcafe0002;
	tcr->gprs[3] = 0xcafe0003;
	tcr->gprs[4] = 0xcafe0012;
#endif /* !DEBUG */
	/* Calls pthread_exit() on return keyword. This might need to be fixed at
	   runtime in the future.    */
	tcr->lr = (u32) pthread_exit /* & (u32) ~1 */;
	tcr->ret_addr = (u32) start_routine & 0xfffffffe;
	tcr->xpsr = xPSR_T_Msk;

	return tcr;
}

struct thread_info *thread_create(void *(*start_routine)(void *), void *arg,
				enum thread_privilege priv, const pthread_attr_t *attr)
{
	struct thread_info *thread;
	struct kernel_context_regs *kcr;
	static int thread_count = 0;
	size_t stacksize;
	struct rlimit stacklimits;

	/* get the thread's stack size */
	if (attr->flags & PTHREAD_ATTR_STACKSIZE) {
		stacksize = attr->stacksize;
	} else {
		getrlimit(RLIMIT_STACK, &stacklimits);
		stacksize = stacklimits.rlim_cur;
	}

	if ((kcr = build_intr_stack()) == NULL)
		return NULL;
	thread = (struct thread_info *) align_lo((u32) kcr, INTR_STACK_SIZE);
	if ((thread->ti_mach.mi_psp = (u32) build_thrd_stack(start_routine, arg, stacksize)) == 0) {
		//FIXME: free(kcr);
		return NULL;
	}
	thread->ti_mach.mi_msp = (u32) kcr;
	thread->ti_mach.mi_priv = priv;
	thread->ti_id = thread_count++;
	/* thread->ti_joinable = false; */

	return thread;
}

int thread_yield(void)
{
#ifdef DEBUG
	CURRENT_THREAD_INFO(thread);
	printk("thread: id=%d is yielding\n", thread->ti_id);
#endif /* DEBUG */

	//FIXME: use a top-level function instead, like sched_elect()
	return sched_rr_elect();
}

int thread_self(void)
{
	CURRENT_THREAD_INFO(thread);

	return thread->ti_id;
}

void thread_exit(void *retval)
{
	CURRENT_THREAD_INFO(thread);

	thread->ti_retval = retval;
	printk("thread: id=%d is exiting with retval=%d\n", thread->ti_id, (int) retval);
	//FIXME: this does not release the resource, and creates a zombie thread
	sched_rr_del(thread);
}
