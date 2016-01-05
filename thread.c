#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include "pthread.h"
#include "page.h"
#include "utils.h"
#include "sched-rr.h"
#include "arch-v7m.h"
#include "linux/types.h"

static struct __intr_stackframe *build_intr_stack(void)
{
	void *memp;
	struct __intr_stackframe *is;

	/* We don't need a huge stack for interrupt handling, however we need the page
	   to be aligned on a known value to retrieve the Thread Control Block that
	   is stored at the bottom of the physical page.    */
	if ((memp = page_alloc()) == NULL)
		return NULL;
	is = (void *)((u32) memp + PAGE_SIZE - sizeof (struct __intr_stackframe));
#ifndef DEBUG
	memset(is->is_gprs, 0, 8 * sizeof (u32));
#else
	is->is_gprs[0] = 0xcafe0004;
	is->is_gprs[1] = 0xcafe0005;
	is->is_gprs[2] = 0xcafe0006;
	is->is_gprs[3] = 0xcafe0007;
	is->is_gprs[4] = 0xcafe0008;
	is->is_gprs[5] = 0xcafe0009;
	is->is_gprs[5] = 0xcafe0010;
	is->is_gprs[7] = 0xcafe0011;
#endif /* !DEBUG */
	is->is_lr = V7M_EXC_RETURN_THREAD_PROCESS;

	return is;
}

// pass user-supplied stack size - configure task stack size
// note: detect stack overflow -> #memf && (SP_Process == MMFAR)
static struct __thrd_stackframe *build_thrd_stack(void *(*entry)(void *), void *args)
{
	void *memp;
	struct __thrd_stackframe *ts;

	if (NULL == (memp = page_alloc())) // this one does not need to be aligned - size configurable
		return NULL;
	ts = (void *)((u32) memp + PAGE_SIZE - sizeof (struct __thrd_stackframe));
	ts->ts_gprs[0] = (u32) args;
#ifndef DEBUG
	memset(&ts->ts_gprs[1], 0, 4 * sizeof (u32));
#else
	printf("thread sp = %p\n", ts);
	ts->ts_gprs[1] = 0xcafe0001;
	ts->ts_gprs[2] = 0xcafe0002;
	ts->ts_gprs[3] = 0xcafe0003;
	ts->ts_gprs[4] = 0xcafe0012;
#endif /* !DEBUG */
	ts->ts_lr = 0x14; //FIXME: address of syscall pthread_exit() - might need to be set at load time
	ts->ts_ret_addr = (u32) entry & (u32) ~1;
	ts->ts_xpsr = V7M_XPSR_T;

	return ts;
}

struct thread_info *thread_create(void *(*entry)(void *), void *args,
				enum thread_privilege priv)
{
	struct thread_info *thread;
	struct __intr_stackframe *is;
	static int thread_count = 0;

	if ((is = build_intr_stack()) == NULL)
		return NULL;
	//printf("intr sp = %p\n", is);
	thread = (struct thread_info *) align_lo((u32) is, PAGE_SIZE);
	if ((thread->ti_psp = build_thrd_stack(entry, args)) == NULL) {
		//FIXME: free(is);
		return NULL;
	}
	thread->ti_entry = entry;
	thread->ti_args = args;
	thread->ti_msp = is;
	thread->ti_priv = priv;
	thread->ti_id = thread_count++;
	/* thread->ti_joinable = false; */

	return thread;
}

int thread_yield(void)
{
	//FIXME: use a top-level function instead, like sched_elect()
	sched_rr_elect();

	return 0;
}

pthread_t thread_self(void)
{
	CURRENT_THREAD_INFO(thread);

	return thread->ti_id;
}

void thread_exit(void *retval)
{
	CURRENT_THREAD_INFO(thread);

	thread->ti_retval = retval;
	printf("in thread exit for id=%d with retval=%d\n", thread->ti_id, (int) retval);
	//FIXME: this does not release the resource, and creates a zombie thread
	sched_rr_del(thread);
}
