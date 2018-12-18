/*
 * kernel/sched.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/bitops.h>
#include <kernel/idle.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/thread.h>
#include <kernel/types.h>
#include <kernel/syscalls.h>

#include <asm/current.h>
#include <asm/switch_to.h>

#define RUNQUEUE_MAX	32

unsigned long		pri_bitmap;
static struct list_head	pri_runq[RUNQUEUE_MAX];

/* pick up the highest priority task */
static struct task_struct *pick_next_task(void)
{
	int max_pri = find_first_bit(&pri_bitmap, 32);

	/* all runqueues are empty, return the idle_thread */
	if (max_pri == 32)
		return idle_task;

	return list_first_entry(&pri_runq[max_pri], struct task_struct, rq);
}

int schedule(void)
{
	struct task_struct *next = pick_next_task();
	struct task_struct *prev = current;

	switch_to(prev, next, prev);

	if (unlikely(prev->state == TASK_DEAD))
		put_task_struct(prev);

	return 0;
}

int set_task_state(struct task_struct *tsk, int state)
{
	if ((state == TASK_RUNNING) && (tsk->state != TASK_RUNNING)) {
		list_add_tail(&tsk->rq, &pri_runq[tsk->prio]);
		bitmap_set_bit(&pri_bitmap, tsk->prio);
	} else if ((state != TASK_RUNNING) && (tsk->state == TASK_RUNNING)) {
		list_del_init(&tsk->rq);
		if (list_empty(&pri_runq[tsk->prio]))
			bitmap_clear_bit(&pri_bitmap, tsk->prio);
	}
	tsk->state = state;

	return 0;
}

int sched_init(void)
{
	/* initialize the runqueues */
	for (int i = 0; i < RUNQUEUE_MAX; i++)
		INIT_LIST_HEAD(&pri_runq[i]);

	return 0;
}

SYSCALL_DEFINE(sched_yield, void)
{
	if (!list_is_singular(&pri_runq[current->prio])) {
		list_del(&current->rq);
		list_add_tail(&current->rq, &pri_runq[current->prio]);
	}

	schedule();

	return 0;
}

int wake_up_process(struct task_struct *tsk)
{
	int need_resched = 0;

	if (tsk->state == TASK_UNINTERRUPTIBLE)
		return -1;

	if (!pri_bitmap)
		need_resched = 1;

	struct task_struct *next = pick_next_task();
	if (next->prio >= tsk->prio)
		need_resched = 1;

	set_task_state(tsk, TASK_RUNNING);

	if (need_resched)
		return schedule();

	return 0;
}
