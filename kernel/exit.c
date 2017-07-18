/*
 * kernel/exit.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/sched.h>

#include <asm/current.h>

void sys_exit(int status)
{
	current->state = EXIT_ZOMBIE;
	current->exit_code = status;
	schedule();

	//XXX: We should send a kernel signal to task's parent, and the
	// parent would release system resources (when? on its next scheduling?
	// Not great if the parent is blocked forever...)
}
