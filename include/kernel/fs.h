/*
 * include/kernel/fs.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _KERNEL_FS_H
#define _KERNEL_FS_H

#include <kernel/types.h>

#include <sys/types.h> // for off_t

#define NAME_MAX 32 // <limits.h>

#define SEEK_SET  0  /* seek relative to beginning of file */
#define SEEK_CUR  1  /* seek relative to current file position */
#define SEEK_END  2  /* seek relative to end of file */

#define S_IFMT   0170000   /* bit mask for the file type bit field */

#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFDIR  0040000
#define S_IFCHR  0020000

#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)

/*
 * inode struct
*/

struct inode_operations;
struct file_operations;

struct inode {
	struct list_head              i_list;        /* list of inodes */
	umode_t                       i_mode;        /* access permissions */
	// kernel_ino_t
	unsigned long                 i_ino;         /* inode number */
	atomic_t                      i_count;       /* reference counter */
	off_t                         i_size;        /* file size in bytes */
	const struct inode_operations *i_op;         /* inode ops table */
	const struct file_operations  *i_fop;        /* default inode ops */
	union {
		void                  *i_private;
		char                  i_data[0];
	};
};

struct dentry;

struct inode_operations {
	struct dentry * (*lookup) (struct inode *inode, struct dentry *dentry);
};

/*
 * file struct
 */

struct file {
	struct dentry                *f_dentry;    /* associated dentry object */
	const struct file_operations *f_op;        /* file operations table */
	off_t                        f_pos;        /* file offset (file pointer) */
	void                         *f_private;
};

struct dir_context;

struct file_operations {
	off_t   (*seek) (struct file *file, off_t offset, int origin);
	ssize_t (*read) (struct file *file, char *buf, size_t count, off_t offset);
	ssize_t (*write) (struct file *file, const char *buf, size_t count, off_t *offset);
	int     (*iterate) (struct file *file, struct dir_context *ctx);
	int     (*open) (struct inode *inode, struct file *file);
};

/*
 * dentry struct
*/

struct dentry {
	_Atomic int                 d_count;            /* usage count */
	struct inode             *d_inode;           /* associated inode */
	//struct dentry_operations *d_op;        /* dentry operations table */
	struct dentry            *d_parent;          /* dentry object of parent */
	/* unsigned */ char            d_name[NAME_MAX];   /* short name */
};

/* readdir */

typedef int (*filldir_t)(struct dir_context *, const char *, int, off_t,
			unsigned int, unsigned int);

struct dir_context {
	const filldir_t actor;
	off_t pos; //XXX: unused because vega_dirent has no offset field
};

struct vega_dirent;

struct readdir_callback {
	struct dir_context ctx;
	struct vega_dirent *dirent;
	int result;
};

struct vega_dirent {
	ino_t d_ino;            /* inode number */
	char  d_name[NAME_MAX]; /* filename */
};
struct dirent {
	ino_t d_ino;            /* inode number */
	char  d_name[NAME_MAX]; /* filename */
};

static inline int dir_emit(struct dir_context *ctx, const char *name,
			int namelen, unsigned int ino, unsigned int type)
{
	return ctx->actor(ctx, name, namelen, ctx->pos, ino, type);
}

static inline int dir_emit_dot(struct file *file, struct dir_context *ctx)
{
	return ctx->actor(ctx, ".", 1, ctx->pos,
			file->f_dentry->d_inode->i_ino, 0);
}

static inline int dir_emit_dotdot(struct file *file, struct dir_context *ctx)
{
	return ctx->actor(ctx, "..", 2, ctx->pos,
			file->f_dentry->d_parent->d_inode->i_ino, 0);
}

/* forward declarations */

int vfs_iterate(struct file *file, struct dir_context *ctx);
struct dentry *vfs_lookup(struct inode *dir, struct dentry *target);

#endif /* !_KERNEL_FS_H */