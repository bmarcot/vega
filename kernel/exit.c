/*
 * kernel/exit.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>

#include "linux/list.h"

void sys_exit(int status)
{
	pr_info("exit_status=%d", status);

	struct task_struct *current = get_current();

	list_del(&current->ti_list);
	/* We are freeing the stack we are running on, no kernel preemption
	 * is allowed until we call sched_elect().  */
	free_pages((unsigned long)current->stack,
		size_to_page_order(THREAD_SIZE));
	sched_elect(SCHED_OPT_RESTORE_ONLY);
}
