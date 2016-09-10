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

/* filedesc */

int fd_find(void);
void fd_reserve(int fd);
void fd_release(int fd);

/* kernel interfaces --- system if */
int open_1(const char *pathname, int flags);
ssize_t read_1(int fd, /* __user */ void *buf, size_t count);
off_t seek_1(int fd, off_t offset, int whence);
int stat_1(const char *pathname, struct stat *buf);
int mount_1(const char *source, const char *target, const char *filesystemtype,
	unsigned long mountflags, const void *data);

/* initialization, object creation, etc. */
void fs_init(void);

/* debug functions */
void ls(void);

#endif /* !KERNEL_FS_FS_H */