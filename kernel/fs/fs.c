/*
 * kernel/fs/fs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/fs.h>
#include <kernel/fs/path.h>
#include <kernel/fs/romfs.h>

//FIXME: the file table is part of the task structure
#define FILE_MAX 8

struct file *fd_to_file(int fd)
{
	static struct file filetable[FILE_MAX];

	return &filetable[fd];
}

unsigned long filemap;

static int getfd(void)
{
	int fd;

	fd = find_first_zero_bit(&filemap, BITS_PER_LONG); // MAX_FILE_PER_PROCESS
	if (fd == BITS_PER_LONG)
		return -1;
	bitmap_set_bit(&filemap, fd);

	return fd;
}

static void releasefd(int fd)
{
	bitmap_clear_bit(&filemap, fd);
}

int release_dentries(struct dentry *dentry)
{
	struct dentry *parent;

	for (; dentry != root_dentry(); dentry = parent) {
		if (!dentry->d_count)
			return -1;
		if (--dentry->d_count)
			break;
		parent = dentry->d_parent;
		vfs_release(dentry);
		if (!dentry->d_count)
			vfs_delete(dentry);
	}

	return 0;
}

struct inode *inode_from_pathname(const char *pathname/* , struct dentry *from */)
{
	struct inode *inode = root_inode();
	struct dentry *dentry = root_dentry();
	struct dentry target;

	/* remove the trailing slash, relative path is not supported */
	pathname++;

	for (size_t i = 0; i < strlen(pathname);) {
		i += path_head(target.d_name, &pathname[i]);
		dentry = vfs_lookup(inode, &target);
		if (dentry == NULL)
			return NULL;
		inode = dentry->d_inode;
	}

	return inode;
}

int sys_open(const char *pathname, int flags)
{
	struct inode *inode = root_inode();
	struct dentry *dentry = root_dentry();
	struct dentry *parent = dentry;
	struct dentry *target;

	/* remove the trailing slash, relative path is not supported */
	pathname++;

	for (size_t i = 0; i < strlen(pathname);) {
		target = malloc(sizeof(struct dentry));
		if (target == NULL)
			return -1;
		target->d_count = 1;
		target->d_parent = parent;
		i += path_head(target->d_name, &pathname[i]);

		dentry = vfs_lookup(inode, target);
		if (dentry == NULL) {
			release_dentries(target);
			return -1;
		}
		inode = dentry->d_inode;
		parent = dentry;
	}

	/* opendir() redirects to sys_open() */
	if ((flags & O_DIRECTORY) && !S_ISDIR(inode->i_mode)) {
		errno = ENOTDIR;
		return -1;
	}

	int fd = getfd();
	struct file *file = fd_to_file(fd);
	file->f_dentry = dentry;
	file->f_op = dentry->d_inode->i_fop;
	file->f_pos = 0;
	if (file->f_op->open)
		file->f_op->open(inode, file);

	return fd;
}

ssize_t sys_read(int fd, void *buf, size_t count)
{
	struct file *file = fd_to_file(fd);

	count = file->f_op->read(file, buf, count, file->f_pos);
	file->f_pos += count;

	return count;
}

ssize_t sys_write(int fd, void *buf, size_t count)
{
	struct file *file = fd_to_file(fd);
	off_t offset = file->f_pos;

	count = file->f_op->write(file, buf, count, &offset);
	file->f_pos += count;

	return count;
}

off_t sys_seek(int fd, off_t offset, int whence)
{
	struct file *file = fd_to_file(fd);
	off_t size = file->f_dentry->d_inode->i_size;

	if (file->f_op->seek)
		file->f_op->seek(file, offset, whence);
	switch (whence) {
	case SEEK_SET:
		if (offset)
			offset = size % offset;
		break;
	case SEEK_CUR:
		offset = (file->f_pos + offset) % size;
		break;
	case SEEK_END:
		offset = (size + offset) % size;
		break;
	default:
		return -1;
	}
	file->f_pos = offset;

	return 0;
}

int sys_close(int fd)
{
	struct file *file = fd_to_file(fd);

	release_dentries(file->f_dentry);
	releasefd(fd);

	return 0;
}

int sys_stat(const char *pathname, struct stat *buf)
{
	(void)pathname, (void)buf;

	return 0;
}

int sys_mount(const char *source, const char *target,
	const char *filesystemtype,
	unsigned long mountflags, const void *data)
{
	if (!strcmp("romfs", filesystemtype))
		return romfs_mount(source, target, filesystemtype, mountflags,
				data);

	return -1;
}
