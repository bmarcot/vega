#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "linux/list.h"
#include "kernel.h"

extern struct thread_info *thread_idle;

static unsigned int pri_bitmap;
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
	int max_pri = __builtin_ctz(pri_bitmap);

	/* all runqueues are empty, return the idle_thread */
	if (max_pri == 32)
		return thread_idle;  // idle_thread

	/* printk("-- max_pri = %d\n", max_pri); */

	return list_first_entry(&pri_runq[max_pri], struct thread_info, ti_q);
}

static int sched_o1_add(struct thread_info *thread)
{
	list_add_tail(&thread->ti_q, &pri_runq[thread->ti_priority]);
	pri_bitmap |= (1 << thread->ti_priority);

	/* printk("-- add %p to runq [%d]\n", thread, thread->ti_priority); */

	return 0;
}

static int sched_o1_del(struct thread_info *thread)
{
	CURRENT_THREAD_INFO(current);

	/* active thread is not in the runqueue */
	if (thread == current)
		return 0;

	list_del(&thread->ti_q);
	if (list_empty(&pri_runq[thread->ti_priority]))
		pri_bitmap &= ~(1 << thread->ti_priority);

	return 0;
}

static int sched_o1_elect(int flags)
{
	CURRENT_THREAD_INFO(current);
	struct thread_info *next;

	/* printk("## sched_elect()\n"); */

	next = find_next_thread();
	/* printk("-- next thread: %p\n", next); */

	if (next != thread_idle) {  // idle_thread
		/* printk("-- remove %p from runq\n", next); */
		list_del(&next->ti_q);
		if (list_empty(&pri_runq[next->ti_priority]))
			pri_bitmap &= ~(1 << next->ti_priority);
	}

	if (flags & SCHED_OPT_RESTORE_ONLY)
		thread_restore(next);  // switch_to_restore_only
	else
		switch_to(next, current);

	return 0;
}

const struct sched sched_o1 = {
	.init = sched_o1_init,
	.add = sched_o1_add,
	.del = sched_o1_del,
	.elect = sched_o1_elect
};
