/*
 * kernel/ioctl.c
 *
 * Copyright (c) 2019 Benoit Marcot
 */

#include <kernel/fs.h>
#include <kernel/syscalls.h>

SYSCALL_DEFINE(ioctl,
	struct file	*file,
	unsigned int	cmd,
	unsigned long	arg)
{
	pr_devel("ioctl(file=%p, cmd=%d, arg=%d)\n", file, cmd, arg);

	return 0;
}
