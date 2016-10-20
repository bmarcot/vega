#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/scheduler.h>
#include <kernel/signal.h>
#include <kernel/time.h>
#include <kernel/thread.h>

#include "linux/list.h"
#include "kernel.h"
#include "systick.h"

/* unordered lists of active/inactive timers */
static LIST_HEAD(timers);
static LIST_HEAD(inactive_timers);

extern u32 clocktime_in_msecs;

struct timer {
	struct timer_common common;
	struct list_head list;
	struct sigevent __sigev[0];
};

static struct timer *find_timer_by_id(timer_t timerid, struct list_head *timer_list)
{
	struct timer *pos;

	list_for_each_entry(pos, timer_list, list) {
		if (pos->common.timerid == timerid)
			return pos;
	}

	return NULL;
}

static int reserve_timer_id(timer_t *timerid)
{
	static unsigned long timerid_bitmap = 0;
	unsigned long bit;

	if (timerid == NULL)
		return -1;
	bit = find_first_zero_bit(&timerid_bitmap, BITS_PER_LONG);
	if (bit == BITS_PER_LONG)
		return -1;
	bitmap_set_bit(&timerid_bitmap, bit);
	*timerid = bit;

	return 0;
}

int __msleep(unsigned int msec)
{
	//FIXME: we are already in the kernel because sleep() is a syscall

	/* XXX: 1 timer per thread, replace wih malloc/free when implementing
	        timer_create/timer_arm/signals timer management. */
	struct timer timer;
	CURRENT_THREAD_INFO(threadp);

	printk("timer: create a timer t=%dms at %p\n", msec, &timer);

	timer.common.owner = threadp;
	timer.common.expire_clocktime = get_clocktime_in_msec() + msec;
	timer.common.timer_type = TT_SLEEP;
	list_add(&timer.list, &timers); // add to list of armed timer
	sched_dequeue(threadp);
	sched_elect(SCHED_OPT_NONE);

	return 0;
}

extern struct thread_info *thread_idle;

void do_sigevent(const struct sigevent *sigevent);

void __systick(unsigned long clocktime_in_msec)
{
	struct timer *pos, *pos1;
	int goto_sleep = 0;

	list_for_each_entry_safe(pos, pos1, &timers, list) {
		if (pos->common.expire_clocktime < clocktime_in_msec) {
			if (pos->common.timer_type == TT_SLEEP) {
				goto_sleep++;
				sched_enqueue(pos->common.owner);
				list_del(&pos->list);
			} else {
				printk("OK! OK! OK! timer exprired!\n");
				/* FIXME: Sigevent handler is staged in current thread
				   at the moment. It should be staged in timer's owner
				   thread instead. */
				do_sigevent(pos->common.sigev);
				// if timer periodic
				//    list_move()
				// else
				list_del(&pos->list);
			}
			/* free(timer); */
		}
	}
	if (goto_sleep) {
		CURRENT_THREAD_INFO(current);
		if (current != thread_idle)
			sched_enqueue(current);
		sched_elect(SCHED_OPT_NONE);
	}
}

int sys_timer_create(clockid_t clockid, struct sigevent *sevp,
		timer_t *timerid)
{
	(void)clockid;

	struct timer *timer;

	printk("Sys Timer Create\n");

	timer = malloc(sizeof(struct timer) + sizeof(struct sigevent));
	if (timer == NULL)
		return -ENOMEM;
	if (reserve_timer_id(&timer->common.timerid)) {
		free(timer);
		return -1;
	}
	*timerid = timer->common.timerid;
	timer->common.timer_type = TT_TIMER;
	timer->common.sigev = timer->__sigev;
	memcpy(timer->__sigev, sevp, sizeof(struct sigevent));
	list_add(&timer->list, &inactive_timers); // add to list of non-armed timer

	return 0;
}

/* int timer_settime(timer_t timerid, int flags, */
/* 		const struct itimerspec *new_value, */
/* 		struct itimerspec * old_value) */
int timer_settime(timer_t timerid, int flags, int new_value)
{
	(void)flags;

	struct timer *timer = find_timer_by_id(timerid, &inactive_timers);

	if (timer == NULL) {
		printk("timer_settime: No timer found with id=%d\n", timerid);
		return -EINVAL;
	}
	timer->common.expire_clocktime = get_clocktime_in_msec() + new_value;
	list_move(&timer->list, &timers);

	return 0;

}
