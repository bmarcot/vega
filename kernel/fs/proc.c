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
	struct inode *proc;

	/* create /proc */
	proc = make_dir(root_inode(), "proc");
	if (!proc) {
		pr_err("Cannot create /proc");
		return;
	}

	/* create /proc/version */
	if (!creat_file(proc, "version", &version_fops))
		pr_warn("Cannot create /proc/version");

	/* create /proc/meminfo */
	if (!creat_file(proc, "meminfo", &meminfo_fops))
		pr_warn("Cannot create /proc/meminfo");
}
