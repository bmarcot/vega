/*
 * include/kernel/fs/initfs.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_FS_INITFS_H
#define _KERNEL_FS_INITFS_H

#include <kernel/types.h>

struct initfs_inode {
	u16	file_size;
	char	file_name[0];
};

int initfs_mount(const char *source, const char *target,
		const char *filesystemtype,
		unsigned long mountflags, const void *data);

#endif /* !_KERNEL_FS_INITFS_H */
