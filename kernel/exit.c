/*
 * kernel/exit.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/sched.h>

#include "linux/list.h"

void sys_exit(int status)
{
	struct task_struct *current = get_current();

	current->state = EXIT_ZOMBIE;
	current->exit_code = status;
	list_del(&current->ti_list);
	sched_elect(SCHED_OPT_RESTORE_ONLY);

	//XXX: We should send a kernel signal to task's parent, and the
	// parent would release system resources (when? on its next scheduling?
	// Not great if the parent is blocked forever...)
}
