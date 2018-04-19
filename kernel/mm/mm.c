/*
 * kernel/mm/mm.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <errno.h>
#include <sys/cdefs.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/mm.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>
#include <kernel/types.h>

#include <asm/current.h>

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

#define max(a, b) ((a) > (b) ? (a) : (b))

SYSCALL_DEFINE(mmap,
	void		*addr,
	size_t		length,
	int		prot,
	int		flags,
	int		fd,
	off_t		offset)
{
	addr = do_mmap(addr, length, prot, flags, fd, offset);

	/* Record process allocations, such that any unreleased memory will be
	 * freed by the kernel when process exits. */
	if (addr) {
		struct mm_struct *mm = current->mm;
		struct mm_region *region = kmalloc(sizeof(*region));

		if (!region) {
			free_pages((unsigned long)addr,
				size_to_page_order(length));
			return (long)MAP_FAILED; //XXX: And set errno to ENOMEM
		}

		mm->size += length;
		mm->max_size = max(mm->max_size, mm->size);

		region->start = addr;
		region->length = length;
		region->flags = flags;
		list_add(&region->list, &mm->region_head);
	}

	return (long)addr;
}

SYSCALL_DEFINE(munmap,
	void		*addr,
	size_t		length)
{
	struct mm_struct *mm = current->mm;
	struct mm_region *region, *n;

	//XXX: Walking the list is too slow, use a BST
	list_for_each_entry_safe(region, n, &mm->region_head, list) {
		if (region->start == addr) {
			mm->size -= region->length;
			list_del(&region->list);
			kfree(region);
		}
	}

	/* Closing the file descriptor does not unmap the region. */
	free_pages((unsigned long)addr, size_to_page_order(length));

	return 0;
}

void mm_release(void)
{
	struct mm_struct *mm = current->mm;
	struct mm_region *region, *n;

	if (!mm->size)
		return;

	if (--mm->refcount > 1)
		return;

	list_for_each_entry_safe(region, n, &mm->region_head, list) {
		if (M_ISANON(region->flags))
			free_pages((unsigned long)region->start,
				size_to_page_order(region->length));
		mm->size -= region->length;
		list_del(&region->list);
		kfree(region);
	}

	BUG_ON(mm->size);
}

struct mm_struct *alloc_mm_struct(void)
{
	struct mm_struct *mm;

	mm = kmalloc(sizeof(*mm));
	if (!mm)
		return NULL;

	mm->size = 0;
	mm->max_size = 0;
	INIT_LIST_HEAD(&mm->region_head);

	return mm;
}
