/*
 * kernel/fs/fs.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/fs.h>
#include <kernel/fs/path.h>

extern struct inode rootdir_inodes[];
extern struct dentry rootdir_dentries[];

//FIXME: the file table is part of the task structure
#define FILE_MAX 8
/* static */ struct file filetable[FILE_MAX];

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

int sys_open(const char *pathname, int flags)
{
	(void)flags;

	char buf[NAME_MAX];
	struct inode *inode = &rootdir_inodes[0]; // fsroot_inode();
	struct dentry target;
	struct dentry *dentry;

	for (size_t i = 0; i < strlen(pathname);) {
		i += path_head(buf, &pathname[i]);
		strcpy(target.d_name, buf);
		dentry = vfs_lookup(inode, &target);
		if (dentry == NULL)
			return -1;
		inode = dentry->d_inode;
	}

	if ((flags & O_DIRECTORY) && !S_ISDIR(inode->i_mode)) {
		errno = ENOTDIR;
		return -1;
	}

	int fd = getfd();
	struct file *file = &filetable[fd];
	file->f_dentry = dentry;
	file->f_op = dentry->d_inode->i_fop;
	file->f_pos = 0;
	if (file->f_op->open)
		file->f_op->open(inode, file);

	return fd;
}

ssize_t sys_read(int fd, void *buf, size_t count)
{
	//FIXME: revalidate the filedesc
	struct file *file = &filetable[fd];

	count = file->f_op->read(file, buf, count, file->f_pos);
	file->f_pos += count;

	return count;
}

ssize_t sys_write(int fd, void *buf, size_t count)
{
	struct file *file = &filetable[fd];
	off_t offset = file->f_pos;

	count = file->f_op->write(file, buf, count, &offset);
	file->f_pos += count;

	return count;
}

off_t sys_seek(int fd, off_t offset, int whence)
{
	struct file *file = &filetable[fd];
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
	(void)fd;

	return 0;
}

int sys_stat(const char *pathname, struct stat *buf)
{
	(void)pathname, (void)buf;

	return 0;
}

int sys_mount(const char *source, const char *target, const char *filesystemtype,
	unsigned long mountflags, const void *data)
{
	(void)source, (void)target, (void)filesystemtype, (void)mountflags, (void)data;

	return 0;
}
