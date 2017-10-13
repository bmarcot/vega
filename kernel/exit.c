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
	/* current task becomes a zombie */
	current->state = EXIT_ZOMBIE;
	current->exit_code = status;

	release_task(current);

	schedule();

	/*
	 * exit() does not return. Task's resources are kept, just in case the
	 * parent thread needs to read child's stack, or the tasks's exit value.
	 * Releasing child task's resources can be done by any other task.
	 */

	//XXX: We should send a kernel signal to task's parent, and the
	// parent would release system resources (when? on its next scheduling?
	// Not great if the parent is blocked forever...)

	return 0; /* never reached */
}
