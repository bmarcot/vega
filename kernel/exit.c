/*
 * kernel/exit.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/sched.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

SYSCALL_DEFINE(exit, int status)
{
	/*
	 * Task becomes a zombie, the task's resources and stack are released
	 * in a different context.
	 */
	current->state = EXIT_ZOMBIE;
	current->exit_code = status;
	schedule();

	//XXX: We should send a kernel signal to task's parent, and the
	// parent would release system resources (when? on its next scheduling?
	// Not great if the parent is blocked forever...)

	return 0; /* exit() does not return */
}
