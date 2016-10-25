/*
 * include/kernel/fs/fs.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_FS_FS_H
#define KERNEL_FS_FS_H

#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/fs/vnode.h>

#define SEEK_SET  0  /* seek relative to beginning of file */
#define SEEK_CUR  1  /* seek relative to current file position */
#define SEEK_END  2  /* seek relative to end of file */

#define FD_MAX  32  /* 32 files can be opened cocurrently  */

#define MAX_FILES_PER_DEV  8

struct file {
	off_t f_pos;
	struct vnode *f_vnode;
};

/* syscall entry points */
int     sys_open(const char *pathname, int flags);
ssize_t sys_read(int fd, void *buf, size_t count);
ssize_t sys_write(int fd, void *buf, size_t count);
off_t   sys_seek(int fd, off_t offset, int whence);
int     sys_stat(const char *pathname, struct stat *buf);
int     sys_mount(const char *source, const char *target,
		  const char *filesystemtype,
		  unsigned long mountflags, const void *data);

/* initialization, object creation, etc. */
void fs_init(void);

/* debug functions */
void ls(void);

#endif /* !KERNEL_FS_FS_H */
