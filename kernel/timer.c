#include <sys/types.h>

#include <kernel/sched-rr.h>
#include <kernel/timer.h>
#include <kernel/thread.h>

#include "linux/list.h"
#include "kernel.h"

/* static */ LIST_HEAD(timers); //XXX: create accessors..

extern u32 clocktime_in_msecs;

int timer_create_1(unsigned int msecs)
{
	/* XXX: 1 timer per thread, replace wih malloc/free when implementing
	        timer_create/timer_arm/signals timer management. */
	struct timer timerp;
	CURRENT_THREAD_INFO(threadp);

	printk("timer: create a timer t=%dms at %p\n", msecs, &timerp);

	timerp.tip = threadp;
	timerp.expire_clocktime = clocktime_in_msecs + msecs;
	list_add(&timerp.list, &timers);
	list_del(&threadp->ti_list);
	sched_rr_elect_reset();

	return 0;
}
