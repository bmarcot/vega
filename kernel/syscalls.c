/*
 * kernel/syscalls.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/kernel.h>

#define __SYSCALL_ARM(nr, sym) 	extern int sys_##sym();
#include <arch/syscalls_arm.h>
#undef __SYSCALL_ARM

#define __SYSCALL_ARM(nr, sym) \
	[nr] = sys_##sym,

void *syscall_vect[48] = {
	#include <arch/syscalls_arm.h>
};

int syscall_register(unsigned ix, void *(*fn)())
{
	if (ix >= ARRAY_SIZE(syscall_vect))
		return -1;
	syscall_vect[ix] = fn;

	return 0;
}
