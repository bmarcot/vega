#include <sys/types.h>

#include <kernel/scheduler.h>
#include <kernel/timer.h> //FIXME: types in libc instead
#include <kernel/thread.h>

#include "linux/list.h"
#include "kernel.h"

/* static */ LIST_HEAD(timers); //XXX: create accessors..

extern u32 clocktime_in_msecs;

int sys_timer_create(unsigned int msecs)
{
	/* XXX: 1 timer per thread, replace wih malloc/free when implementing
	        timer_create/timer_arm/signals timer management. */
	struct timer timerp;
	CURRENT_THREAD_INFO(threadp);

	printk("timer: create a timer t=%dms at %p\n", msecs, &timerp);

	timerp.tip = threadp; //FIXME: rename to timer.owner
	timerp.expire_clocktime = clocktime_in_msecs + msecs;
	list_add(&timerp.list, &timers);
	sched_del(threadp);
	sched_elect(SCHED_OPT_NONE);

	return 0;
}
