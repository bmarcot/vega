/*
 * kernel/mm/mm.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <errno.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <vega/sys/mman.h>

#include <kernel/fs.h>
#include <kernel/mm/page.h>

#define M_ISANON(f)     (((f) & MAP_ANONYMOUS)     == MAP_ANONYMOUS)
#define M_ISUNINIT(f)   (((f) & MAP_UNINITIALIZED) == MAP_UNINITIALIZED)

void *sys_mmap(void *addr, size_t length, int prot, int flags, int fd,
	off_t offset)
{
	if (!length) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	if (prot & PROT_NONE) {
		errno = EACCES;
		return MAP_FAILED;
	}
	if (M_ISANON(flags)) {
		int order = size_to_page_order(length);
		addr = alloc_pages(order);
		if (addr == NULL) {
			errno = ENOMEM;
			return MAP_FAILED;
		}
		if (!M_ISUNINIT(flags))
			memset(addr, 0, length);
	} else {
		struct file *file = fd_to_file(fd);
		if (!file) {
			errno = EBADF;
			return MAP_FAILED;
		}
		if (!S_ISREG(file->f_dentry->d_inode->i_mode)) {
			errno = EACCES;
			return MAP_FAILED;
		}
		if (vfs_mmap(file, offset, &addr))
			return MAP_FAILED;
	}

	return addr;
}

int sys_munmap(__unused void *addr, __unused size_t length)
{
	/* Closing the file descriptor does not unmap the region. */

	return 0;
}
