/*
 * kernel/mm/mm.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <errno.h>
#include <string.h>
#include <sys/cdefs.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/stat.h>
#include <kernel/syscalls.h>
#include <kernel/types.h>

#include <uapi/kernel/mman.h>

#define M_ISANON(f)     (((f) & MAP_ANONYMOUS)     == MAP_ANONYMOUS)
#define M_ISUNINIT(f)   (((f) & MAP_UNINITIALIZED) == MAP_UNINITIALIZED)

static void *map_anon(void *addr, size_t length, __unused int prot, int flags)
{
	int order;

	order = size_to_page_order(length);
	addr = alloc_pages(order);
	if (!addr) {
		errno = ENOMEM;
		return MAP_FAILED;
	}
	if (!M_ISUNINIT(flags))
		memset(addr, 0, length);

	return addr;
}

static void *map_file(__unused size_t length, __unused int prot,
		__unused int flags, int fd, off_t offset)
{
	void *addr;
	struct file *file;

	file = fget(fd);
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

	return addr;
}

void *do_mmap(void *addr, size_t length, int prot, int flags, int fd,
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
	if (M_ISANON(flags))
		addr = map_anon(addr, length, prot, flags);
	else
		addr = map_file(length, prot, flags, fd, offset);

	return addr;
}

SYSCALL_DEFINE(mmap,
	void		*addr,
	size_t		length,
	int		prot,
	int		flags,
	int		fd,
	off_t		offset)
{
	return (int)do_mmap(addr, length, prot, flags, fd, offset);
}

SYSCALL_DEFINE(munmap,
	void		*addr,
	size_t		length)
{
	/* Closing the file descriptor does not unmap the region. */
	free_pages((unsigned long)addr, size_to_page_order(length));

	return 0;
}
