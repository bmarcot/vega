/*
 * kernel/sched-o1.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/bitops.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/list.h"
#include "kernel.h"

extern struct thread_info *thread_idle;

static unsigned long pri_bitmap;
static struct list_head pri_runq[32];

static int sched_o1_elect(int flags);

static int sched_o1_init(void)
{
	for (int i = PRI_MAX; i <= PRI_MIN; i++)
		INIT_LIST_HEAD(&pri_runq[i]);

	return 0;
}

static struct thread_info *find_next_thread(void)
{
	int max_pri = find_first_bit(&pri_bitmap, 32);

	/* all runqueues are empty, return the idle_thread */
	if (max_pri == 32)
		return thread_idle;  // idle_thread

	struct thread_struct *next =
		list_first_entry(&pri_runq[max_pri], struct thread_struct, ti_q);
	return next->info;
}

static int sched_o1_enqueue(struct thread_info *thread)
{
	struct thread_struct *thread_struct = thread->ti_struct;

	list_add_tail(&thread_struct->ti_q, &pri_runq[thread_struct->ti_priority]);
	bitmap_set_bit(&pri_bitmap, thread_struct->ti_priority);

	return 0;
}

static int sched_o1_dequeue(struct thread_info *thread)
{
	/* active thread is not in the runqueue */
	struct thread_info *curr_thread = current_thread_info();
	if (thread == curr_thread)
		return 0;

	struct thread_struct *thread_struct = thread->ti_struct;
	list_del(&thread_struct->ti_q);
	if (list_empty(&pri_runq[thread_struct->ti_priority]))
		bitmap_clear_bit(&pri_bitmap, thread_struct->ti_priority);

	return 0;
}

static int sched_o1_elect(int flags)
{
	struct thread_info *next_thread = find_next_thread();
	if (next_thread != thread_idle) {  // idle_thread
		struct thread_struct *thread_struct = next_thread->ti_struct;
		list_del(&thread_struct->ti_q);
		if (list_empty(&pri_runq[thread_struct->ti_priority]))
			bitmap_clear_bit(&pri_bitmap, thread_struct->ti_priority);
	}

	struct thread_info *curr_thread = current_thread_info();
	if (flags & SCHED_OPT_RESTORE_ONLY)
		thread_restore(next_thread);  // switch_to_restore_only
	else
		switch_to(next_thread, curr_thread);

	return 0;
}

const struct sched sched_o1 = {
	.init = sched_o1_init,
	.enqueue = sched_o1_enqueue,
	.dequeue = sched_o1_dequeue,
	.elect = sched_o1_elect
};
