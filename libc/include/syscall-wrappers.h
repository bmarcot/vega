#pragma once

#include <asm/posix_types.h>
#include <asm/syscalls.h>

#define __always_inline inline __attribute__((always_inline))

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

static __always_inline void _Exit(int status)
{
	__asm__ __volatile__ (
		"	mov r1, %0	\r\n"
		"	svc #1		"
		:
		: "I" (SYS_EXIT_GROUP));
}
