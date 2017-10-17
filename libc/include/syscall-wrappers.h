#pragma once

#include <asm/posix_types.h>
#include <asm/syscalls.h>

#define __always_inline inline __attribute__((always_inline))

/* vfork() is special in that the behavior is undefined if the process created
 * by vfork() returns from the function in which vfork() was called. */
static __always_inline pid_t vfork(void)
{
	pid_t pid;

	__asm__ __volatile__ (
		"	mov r0, %1	\r\n"
		"	svc #0		\r\n"
		"	mov %0, r0	"
		: "=r" (pid)
		: "I" (SYS_VFORK));

	return pid;
}

int do_syscall0();
int do_syscall1();
int do_syscall2();
int do_syscall3();
int do_syscall4();
int do_syscall5();
int do_syscall6();

#define _Exit(...)		(void)do_syscall1(__VA_ARGS__, SYS_EXIT_GROUP)
#define _exit_thread(...)	(void)do_syscall1(__VA_ARGS__, SYS_EXIT)
