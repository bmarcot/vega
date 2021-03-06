/*
 * kernel/fs/fs.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <errno.h>

#include <kernel/bitops.h>
#include <kernel/dcache.h>
#include <kernel/errno-base.h>
#include <kernel/fdtable.h>
#include <kernel/fs.h>
#include <kernel/fs/initfs.h>
#include <kernel/fs/path.h>
#include <kernel/fs/romfs.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

struct file *fget(unsigned int fd)
{
	return current->files->fd_array[fd];
}

static struct file *fput(unsigned int fd, struct file *file)
{
	struct file *old_file;

	old_file = current->files->fd_array[fd];
	current->files->fd_array[fd] = file;

	return old_file;
}

static int alloc_fd(void)
{
	int fd;

	fd = find_first_zero_bit(&current->files->fdtab, FILE_MAX);
	if (fd == FILE_MAX)
		return -1;
	bitmap_set_bit(&current->files->fdtab, fd);

	return fd;
}

static void release_fd(int fd)
{
	bitmap_clear_bit(&current->files->fdtab, fd);
}

int validate_fd(unsigned int fd)
{
	if (fd >= FILE_MAX)
		return -1;

	return 0;
}

static struct dentry *dentry_open(struct inode *dir, const char *name)
{
	struct dentry *dentry;

	dentry = d_lookup(dir->i_dentry, name);
	if (!dentry) {
		//FIXME: Could alloc the dentry on the stack, then call d_instantiate
		dentry = d_alloc(dir->i_dentry, name);
		vfs_lookup(dir, dentry);
	}
	d_get(dentry);

	return dentry;
}

#define PATH_MAX 256

struct dentry *__do_file_open(const char *pathname, int flags)
{
	LIST_HEAD(path);
	char *buf;
	struct pathcomp *comp;
	struct dentry *parent = root_dentry();
	struct dentry *dentry = NULL;
	size_t pathlen = strlen(pathname);

	if (pathlen > PATH_MAX)
		return NULL;

	/* Duplicate string */
	buf = kmalloc(pathlen + 1);
	if (!buf) {
		BUG();
		return NULL;
	}
	strcpy(buf, pathname);

	path_split(&path, buf);
	list_for_each_entry(comp, &path, list) {
		dentry = dentry_open(parent->d_inode, comp->name);
		if (dentry) {
			parent = dentry;
		} else {
			d_put_from(parent);
			break;
		}
	}
	path_release(&path);
	kfree(buf);

	return dentry;
}

struct file *do_file_open(const char *pathname, int flags)
{
	struct dentry *dentry;

	dentry = __do_file_open(pathname, flags);
	if (!dentry) {
		/* FILENOTFOUND */
	}

	/* opendir() redirects to open() */
	if ((flags & O_DIRECTORY) && !S_ISDIR(D_INODE(dentry)->i_mode)) {
		errno = ENOTDIR;
		return NULL;
	}

	struct file *file = alloc_file();
	if (file == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	file->f_dentry = dentry;
	file->f_op = D_INODE(dentry)->i_fop;
	file->f_pos = 0;

	if (file->f_op->open)
		file->f_op->open(D_INODE(dentry), file);

	return file;
}

SYSCALL_DEFINE(open,
	const char	*pathname,
	int		flags)
{
	int fd = alloc_fd();
	if (fd < 0)
		return fd;

	struct file *file = do_file_open(pathname, flags);
	if (file == NULL) {
		release_fd(fd);
		return -1;
	}
	fput(fd, file);

	return fd;
}

ssize_t do_file_read(struct file *file, void *buf, size_t count)
{
	if (count)
		count = file->f_op->read(file, buf, count, file->f_pos);
	file->f_pos += count;

	return count;
}

SYSCALL_DEFINE(read,
	int		fd,
	void		*buf,
	size_t		count)
{
	struct file *file = fget(fd);

	return do_file_read(file, buf, count);
}

ssize_t do_file_write(struct file *file, void *buf, size_t count)
{
	off_t offset = file->f_pos;

	count = file->f_op->write(file, buf, count, &offset);
	file->f_pos += count;

	return count;
}

SYSCALL_DEFINE(write,
	int		fd,
	void		*buf,
	size_t		count)
{
	struct file *file = fget(fd);

	return do_file_write(file, buf, count);
}

off_t do_file_lseek(struct file *file, off_t offset, int whence)
{
	off_t size = file->f_dentry->d_inode->i_size;

	if (file->f_op->lseek)
		file->f_op->lseek(file, offset, whence);
	switch (whence) {
	case SEEK_SET:
		if (offset)
			offset = offset % size;
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

SYSCALL_DEFINE(lseek,
	int		fd,
	off_t		offset,
	int		whence)
{
	struct file *file = fget(fd);

	return do_file_lseek(file, offset, whence);
}

int do_file_close(struct file *file)
{
	d_put_from(file->f_dentry);
	put_file(file);

	return 0;
}

SYSCALL_DEFINE(close, int fd)
{
	struct file *file = fget(fd);

	fput(fd, NULL);
	release_fd(fd);

	return do_file_close(file);
}

SYSCALL_DEFINE(stat,
	const char	*pathname,
	struct stat	*buf)
{
	struct dentry *dentry;
	struct inode *inode;

	dentry = __do_file_open(pathname, 0);
	if (!dentry) {
		errno = ENOENT;
		return -1;
	}

	inode = dentry->d_inode;
	buf->st_ino = inode->i_ino;
	buf->st_mode = inode->i_mode;
	buf->st_size = inode->i_size;
	if (S_ISCHR(inode->i_mode))
		buf->st_rdev = inode->i_rdev;
	else
		buf->st_rdev = 0;

	return 0;
}

int do_filesystem_mount(const char *source, const char *target,
			const char *filesystemtype,
			unsigned long mountflags, const void *data)
{
	int retval = -1;

	if (!strcmp("romfs", filesystemtype))
		retval = romfs_mount(source, target, filesystemtype, mountflags, data);
	if (!strcmp("initfs", filesystemtype))
		retval = initfs_mount(source, target, filesystemtype, mountflags, data);

	return retval;
}

SYSCALL_DEFINE(mount,
	const char	*source,
	const char	*target,
	const char	*filesystemtype,
	unsigned long	mountflags,
	const void	*data)
{
	return do_filesystem_mount(source, target, filesystemtype, mountflags, data);
}

SYSCALL_DEFINE(umount, const char *target)
{
	struct dentry *dentry = __do_file_open(target, 0);
	struct super_block *sb;

	sb = dentry->d_inode->i_sb;
	sb->s_op->put_super(sb);

	return 0;
}
