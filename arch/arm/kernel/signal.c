/*
 * arch/arm/kernel/signal.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/sched.h>

#include <asm/current.h>
#include <asm/thread_info.h>

void update_retval(int);
void do_signal(int, int);

void do_notify_resume(int syscall_retcode)
{
	/* save the syscall return value into the syscall frame */
	update_retval(syscall_retcode);

	if (/* thread_info_flags */ current_thread_info()->flags & _TIF_SIGPENDING)
		do_signal(current->sigpending, current->sigval);
}

void do_notify(void)
{
	if (current_thread_info()->flags & _TIF_SIGPENDING)
		do_signal(current->sigpending, current->sigval);
}
