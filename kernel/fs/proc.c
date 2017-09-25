/*
 * kernel/fs/proc.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <string.h>
#include <sys/param.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

// https://linux.die.net/lkmpg/x861.html
// https://lwn.net/Articles/22355/

extern const char _version_ptr;
extern const int  _version_len;

static int open_version(__unused struct inode *inode,
			__unused struct file *file)
{
	return 0;
}

static ssize_t read_version(__unused struct file *file, char *buf,
			size_t count, off_t offset)
{
	size_t n = MIN(count, (int)&_version_len - offset);

	strncpy(buf, &_version_ptr + offset, n);

	return n;
}

static int open_meminfo(__unused struct inode *inode,
			__unused struct file *file)
{
	return 0;
}

static ssize_t read_meminfo(struct file *file, char *buf, size_t count, off_t offset)
{
	(void)file;

	memcpy(buf, (void *)offset, count);

	return count;
}

static const struct file_operations version_fops = {
	.open = open_version,
	.read = read_version,
};

static const struct file_operations meminfo_fops = {
	.open = open_meminfo,
	.read = read_meminfo,
};

void procfs_init(void)
{
	struct inode *proc_i;
	struct inode *target_i;
	struct dentry target_d;

	/* create /proc */
	strcpy(target_d.d_name, "proc");
	proc_i = __tmpfs_create(root_inode(), &target_d, S_IFDIR);

	/* create /proc/version */
	strcpy(target_d.d_name, "version");
	target_i = __tmpfs_create(proc_i, &target_d, S_IFDIR);
	target_i->i_fop = &version_fops;

	/* create /proc/meminfo */
	strcpy(target_d.d_name, "meminfo");
	target_i = __tmpfs_create(proc_i, &target_d, S_IFDIR);
	target_i->i_fop = &meminfo_fops;
}
