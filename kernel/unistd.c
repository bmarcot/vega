/*
 * kernel/unistd.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/sched.h>

#include <asm/current.h>

int sys_getpid(void)
{
	return current->pid;
}
