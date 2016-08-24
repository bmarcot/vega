#include <sys/types.h>

#include <kernel/scheduler.h>
#include <kernel/timer.h> //FIXME: types in libc instead
#include <kernel/thread.h>

#include "linux/list.h"
#include "kernel.h"
#include "systick.h"

static LIST_HEAD(timers);

extern u32 clocktime_in_msecs;

int sys_timer_create(unsigned int msec)
{
	/* XXX: 1 timer per thread, replace wih malloc/free when implementing
	        timer_create/timer_arm/signals timer management. */
	struct timer timerp;
	CURRENT_THREAD_INFO(threadp);

	printk("timer: create a timer t=%dms at %p\n", msec, &timerp);

	timerp.owner = threadp;
	timerp.expire_clocktime = get_clocktime_in_msec() + msec;
	list_add(&timerp.list, &timers);
	sched_dequeue(threadp);
	sched_elect(SCHED_OPT_NONE);

	return 0;
}

int __msleep(unsigned int msec)
{
	//FIXME: we are already in the kernel because sleep() is a syscall
	sys_timer_create(msec);

	return 0;
}

extern struct thread_info *thread_idle;

void __systick(unsigned long clocktime_in_msec)
{
	struct timer *pos;

	list_for_each_entry(pos, &timers, list) {
		if (pos->expire_clocktime < clocktime_in_msec) {
			/* printk("timer expired!\n"); */
			list_del(&pos->list);
			sched_enqueue(pos->owner);
			CURRENT_THREAD_INFO(current);
			if (current != thread_idle)
				sched_enqueue(current);
			sched_elect(SCHED_OPT_NONE);
			return;
		}
	}
}
