/*
 * kernel/fs/ioctl.c
 *
 * Copyright (c) 2019 Benoit Marcot
 */

#include <kernel/fs.h>
#include <kernel/syscalls.h>

#include <errno.h> //FIXME

SYSCALL_DEFINE(ioctl,
	unsigned int fd,
	unsigned int cmd,
	unsigned long arg)
{
	int ret = 0;
	struct file *file;

	file = fget(fd);
	if (!file) {
		errno = EBADF;
		return -1;
	}

	if (file->f_op->ioctl)
		ret = file->f_op->ioctl(file, cmd, arg);

	return ret;
}
