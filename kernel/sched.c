/*
 * kernel/sched.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/bitops.h>
#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <asm/current.h>

#include "linux/list.h"

extern struct task_struct *idle_task;

static unsigned long pri_bitmap;
static struct list_head pri_runq[32];

/* pick up the highest-prio task */
static struct task_struct *pick_next_task(void)
{
	int max_pri = find_first_bit(&pri_bitmap, 32);

	/* all runqueues are empty, return the idle_thread */
	if (max_pri == 32)
		return idle_task;

	struct task_struct *next =
		list_first_entry(&pri_runq[max_pri], struct task_struct, ti_q);

	return next;
}

int sched_enqueue(struct task_struct *task)
{
	task->ti_state = THREAD_STATE_READY;
	task->state = TASK_RUNNING;
	list_add_tail(&task->ti_q, &pri_runq[task->prio]);
	bitmap_set_bit(&pri_bitmap, task->prio);

	return 0;
}

int sched_dequeue(struct task_struct *task)
{
	/* active task is not in the runqueue */
	if (task == current)
		return 0;

	list_del(&task->ti_q);
	if (list_empty(&pri_runq[task->prio]))
		bitmap_clear_bit(&pri_bitmap, task->prio);

	return 0;
}

#include <asm/switch_to.h>

#define CLONE_THREAD 1

int schedule(void)
{
	struct task_struct *next = pick_next_task();
	if (next != idle_task) {
		list_del(&next->ti_q);
		if (list_empty(&pri_runq[next->prio]))
			bitmap_clear_bit(&pri_bitmap, next->prio);
	}

	next->ti_state = THREAD_STATE_RUNNING; //XXX: will die

	struct task_struct *prev = get_current();
	switch_to(prev, next, prev);

	if ((prev->state == EXIT_ZOMBIE) && (prev->flags == CLONE_THREAD))
		free_pages((unsigned long)prev->stack,
			size_to_page_order(THREAD_SIZE));

	return 0;
}

int sched_elect(__unused int flags)
{
	return schedule();
}

static unsigned int idle_stack[32];
struct task_struct *idle_task;

void __do_idle(void);

static __attribute__((noreturn)) int do_idle(__unused void *arg)
{
	for (;;)
		__do_idle();
}

int sched_init(void)
{
	/* initialize the runqueues */
	for (int i = PRI_MAX; i <= PRI_MIN; i++)
		INIT_LIST_HEAD(&pri_runq[i]);

	/* idle_task is not added to the runqueue */
	idle_task = clone_task(do_idle, &idle_stack[32], 0, NULL);
	if (idle_task == NULL) {
		pr_err("Could not create the idle task");
		return -1;
	}
	pr_info("Created idle_thread at <%p> with pid=%d", idle_task, idle_task->pid);

	return 0;
}

static struct list_head tasks;

int init_task(struct task_struct *task)
{
	static int pid = 9000;

	task->prio = PRI_MIN;
	task->ti_state = THREAD_STATE_NEW;

	task->stack = &task->thread_info;
	task->pid = pid++;
	task->filemap = 0;
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;

	list_add(&task->ti_list, &tasks);

	return 0;
}

int sys_sched_yield(void)
{
	sched_enqueue(current);
	schedule();

	return 0;
}
