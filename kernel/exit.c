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

	current->state = EXIT_ZOMBIE;

	list_del(&current->ti_list);
	sched_elect(SCHED_OPT_RESTORE_ONLY);

	//FIXME: we should notify parent before releaseing system resources
}
