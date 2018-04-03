/*
 * include/kernel/fs.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_FS_H
#define _KERNEL_FS_H

#include <kernel/stat.h>
#include <kernel/types.h>

#include <uapi/kernel/dirent.h>
#include <uapi/kernel/fs.h>

#define NAME_MAX 32 //FIXME: Include <limits.h>
#define FILE_MAX 8

#define O_DIRECTORY 1

/*
 * super_block struct
 */

struct dentry;
struct inode; //FIXME: delete me from here, using s_root dentry

struct super_block {
	dev_t        s_dev;      /* identifier */
	struct inode *s_iroot;   //FIXME: just use s_root and dentry
};

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
	unsigned int                  i_count;       /* reference counter */
	kdev_t                        i_rdev;        /* real device node */
	off_t                         i_size;        /* file size in bytes */
	const struct inode_operations *i_op;         /* inode ops table */
	const struct file_operations  *i_fop;        /* default inode ops */
	struct super_block            *i_sb;         /* associated superblock */
	void                          *i_private;
	char                          i_data[0];
};

struct inode_operations {
	struct dentry * (*lookup) (struct inode *inode, struct dentry *dentry);
	int             (*create) (struct inode *dir, struct dentry *dentry,
				umode_t mode, int /* bool */ exclusive);
	int             (*link) (struct dentry *old_dentry, struct inode *dir,
				struct dentry *dentry);
	int             (*mkdir) (struct inode *inode, struct dentry *dentry,
				int mode);
	int             (*mknod) (struct inode *dir, struct dentry *dentry,
				umode_t mode, dev_t dev);
};

/*
 * file struct
 */

struct file {
	struct dentry                *f_dentry;    /* associated dentry object */
	const struct file_operations *f_op;        /* file operations table */
	fmode_t                      f_mode;       /* file access mode */
	off_t                        f_pos;        /* file offset (file pointer) */
	void                         *f_private;
};

struct dir_context;

struct file_operations {
	off_t   (*lseek) (struct file *file, off_t offset, int origin);
	ssize_t (*read) (struct file *file, char *buf, size_t count, off_t offset);
	ssize_t (*write) (struct file *file, const char *buf, size_t count, off_t *offset);
	int     (*iterate) (struct file *file, struct dir_context *ctx);
	int     (*mmap) (struct file *file, off_t offset, void **addr); /* struct vm_area_struct *area */
	int     (*open) (struct inode *inode, struct file *file);
};

/*
 * dentry struct
 */

struct dentry {
	_Atomic int                    d_count;           /* usage count */
	struct inode                   *d_inode;          /* associated inode */
	const struct dentry_operations *d_op;             /* dentry operations table */
	struct dentry                  *d_parent;         /* dentry object of parent */
	char                           d_name[NAME_MAX];  /* short name */

	//struct list_head d_child;       /* child of parent list */
	//struct list_head d_subdirs;     /* our children */
};

struct dentry_operations {
	int  (*delete) (struct dentry *dentry);
	void (*release) (struct dentry *dentry);
};

/*
 * readdir
 */

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

typedef void DIR;

/*
 * VFS-layer forward declarations
 */

int           vfs_iterate(struct file *file, struct dir_context *ctx);
struct dentry *vfs_lookup(struct inode *dir, struct dentry *target);
int           vfs_link(struct dentry *old_dentry, struct inode *dir,
		struct dentry *dentry);
int           vfs_delete(struct dentry *dentry);
void          vfs_release(struct dentry *dentry);
int           vfs_mmap(struct file *file, off_t offset, void **addr);
int           vfs_mkdir(struct inode *inode, struct dentry *dentry, int mode);

/*
 * Misc functions (most will die)
 */

struct inode *root_inode(void);
struct inode *dev_inode(void);
struct inode *proc_inode(void);
struct dentry *root_dentry(void);
struct file *fget(unsigned int fd);
struct inode *inode_from_pathname(const char *pathname);
struct inode *__tmpfs_create(struct inode *dir, struct dentry *dentry,
			int mode);

void tmpfs_init(void);
void procfs_init(void);
int devfs_init(void);

/*
 * Kernel common file-manipulation functions
 */

struct file	*do_file_open(const char *pathname, int flags);
ssize_t		do_file_read(struct file *file, void *buf, size_t count);
ssize_t		do_file_write(struct file *file, void *buf, size_t count);
off_t		do_file_lseek(struct file *file, off_t offset, int whence);
int		do_file_close(struct file *file);
int		do_filesystem_mount(const char *source, const char *target,
			const char *filesystemtype,
			unsigned long mountflags, const void *data);

/*
 * Filesystem helper functions
 */

struct inode *creat_file(struct inode *dir, const char *filename);
struct inode *make_dir(struct inode *dir, const char *filename);
struct inode *make_nod(struct inode *dir, const char *filename, umode_t mode,
		dev_t dev);

/*
 * Inode helper functions
 */

void init_special_inode(struct inode *inode, umode_t mode, kdev_t rdev);
ino_t alloc_inode_ino(void);

#endif /* !_KERNEL_FS_H */
