/*
 * kernel/sched-o1.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/bitops.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include "linux/list.h"

extern struct thread_info *thread_idle;

static unsigned long pri_bitmap;
static struct list_head pri_runq[32];

/* pick up the highest-prio task */
static struct task_struct *pick_next_task(void)
{
	int max_pri = find_first_bit(&pri_bitmap, 32);

	/* all runqueues are empty, return the idle_thread */
	if (max_pri == 32)
		return thread_idle->task;  //FIXME: idle_task

	struct task_struct *next =
		list_first_entry(&pri_runq[max_pri], struct task_struct, ti_q);

	return next;
}

int sched_enqueue(struct task_struct *task)
{
	task->ti_state = THREAD_STATE_READY;
	list_add_tail(&task->ti_q, &pri_runq[task->ti_priority]);
	bitmap_set_bit(&pri_bitmap, task->ti_priority);

	return 0;
}

int sched_dequeue(struct task_struct *task)
{
	struct task_struct *current = get_current();

	/* active task is not in the runqueue */
	if (task == current)
		return 0;

	list_del(&task->ti_q);
	if (list_empty(&pri_runq[task->ti_priority]))
		bitmap_clear_bit(&pri_bitmap, task->ti_priority);

	return 0;
}

int sched_elect(int flags)
{
	KERNEL_STACK_CHECKING;

	struct task_struct *next = pick_next_task();
	if (next != thread_idle->task) {  //FIXME: idle_task
		list_del(&next->ti_q);
		if (list_empty(&pri_runq[next->ti_priority]))
			bitmap_clear_bit(&pri_bitmap, next->ti_priority);
	}

	struct task_struct *current = get_current();
	if (flags & SCHED_OPT_RESTORE_ONLY)
		thread_restore(&next->stack->thread_info);  // switch_to_restore_only
	else
		switch_to(&next->stack->thread_info,
			&current->stack->thread_info);
	current->ti_state = THREAD_STATE_RUNNING;

	return 0;
}

int sched_init(void)
{
	for (int i = PRI_MAX; i <= PRI_MIN; i++)
		INIT_LIST_HEAD(&pri_runq[i]);

	return 0;
}
