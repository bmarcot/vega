/*
 * include/kernel/dcache.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_DCACHE_H
#define _KERNEL_DCACHE_H

#include <kernel/types.h>

#define NAME_MAX 32 //FIXME: Include <limits.h>

struct inode;

struct dentry {
	_Atomic int			d_count;           /* usage count */
	struct inode			*d_inode;          /* associated inode */
	const struct dentry_operations	*d_op;             /* dentry operations table */
	struct dentry			*d_parent;         /* dentry object of parent */
	char				d_name[NAME_MAX];  /* short name */
	struct list_head		d_child;           /* child of parent list */
	struct list_head		d_subdirs;         /* our children */
};

struct dentry_operations {
	int  (*delete) (struct dentry *dentry);
	void (*release) (struct dentry *dentry);
};

struct dentry	*__d_alloc(struct dentry *parent, const char *name);
struct dentry	*d_alloc(struct dentry *parent, const char *name);
void		d_put(struct dentry *dentry);
void		d_instantiate(struct dentry *dentry, struct inode *inode);
struct dentry	*d_lookup(const struct dentry *parent, const char *name);

int		dcache_init(void); //dcache_init

#define D_INODE(dentry) ((dentry)->d_inode)

#endif /* !_KERNEL_DCACHE_H */
