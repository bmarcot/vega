/*
 * kernel/time.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/errno-base.h>
#include <kernel/hrtimer.h>
#include <kernel/ktime.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>
#include <kernel/time.h>

#include <kernel/clockevents.h> // see comment below

#include <asm/current.h>

struct hrtimer_sleeper {
	struct hrtimer timer;
	struct task_struct *task;
};

static enum hrtimer_restart nanosleep_callback(struct hrtimer *timer)
{
	struct hrtimer_sleeper *hs =
		container_of(timer, struct hrtimer_sleeper, timer);

	set_task_state(hs->task, TASK_RUNNING);

	// set TIF_RESCHED if runqueue is empty? do sched_yield() from userland
	//schedule();

	return HRTIMER_NORESTART;
}

SYSCALL_DEFINE(nanosleep,
	const struct timespec	*req,
	struct timespec		*rem)
{
	struct hrtimer_sleeper hs;
	struct hrtimer *timer = &hs.timer;

	if ((req->tv_nsec < 0) || (req->tv_nsec > 999999999))
		return -EINVAL;

	hrtimer_init(timer);
	timer->function = nanosleep_callback;
	hs.task = current;
	hrtimer_start(timer, timespec_to_ktime(*req));

	set_current_state(TASK_INTERRUPTIBLE);

	schedule();

	if (signal_pending(current)) {
		// does not work if more than one timer between head and this timer
		ktime_t elapsed = clockevents_read_elapsed(timer->dev);
		if (rem)
			*rem = ktime_to_timespec(timer->expires - elapsed);
		return -EINTR;
	}

	if (rem) {
		rem->tv_sec = 0;
		rem->tv_nsec = 0;
	}

	return 0;
}
