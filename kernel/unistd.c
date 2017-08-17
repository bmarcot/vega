/*
 * kernel/unistd.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/sched.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

SYSCALL_DEFINE(getpid, void)
{
	return current->pid;
}
