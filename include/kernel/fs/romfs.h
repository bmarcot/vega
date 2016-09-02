/*
 * include/kernel/fs/romfs.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_FS_ROMFS_H
#define KERNEL_FS_ROMFS_H

#include <kernel/types.h> /* linux/types.h    */ // for u32
#include "linux/list.h"  /* linux/list.h     */

#include <kernel/fs/vfs.h>
#include <kernel/fs/vnode.h>

/* The ROMFS specification tells multi-byte values (16/32 bit words) must be
 * in big-endian order.  The ARM architecture is little-endian, such that all
 * words must be translated from 'network' to 'host'.  */

/* Solaris introduced in-memory (or in-core) inodes, and on-device inodes.
   The on-device inode is embedded in an in-core inode. The naming for the
   corresponding structures are:

       in-memory inode    struct inode
       in-core inode      struct icommon

   http://ptgmedia.pearsoncmg.com/images/0131482092/samplechapter/mcdougall_ch15.pdf */

/* on-device inode filesystem root */
//struct __romfs_info {
// romfs_sb
// struct romfs_superblock
//     rs_magic_number
struct superblock {
	u8 magic_number[8];
	u32 full_size;
	u32 checksum;
	char volume_name[0];
	//char data[0];
};

//FIXME: rename to superblock
/* in-memory inode filesystem root */
//struct romfs_info {
struct romfs_root {
	struct superblock *r_superblock;
	char *r_volume; // 'volume_name' in specs // move this into on-device node??
	struct list_head r_inodes;
};

/* on-device inode filesystem entry */
struct icommon {
	u32 ic_next;     /* field 'next_filehdr' in romFS specs */
	u32 ic_info;     /* field 'spec_info' in romFS specs */
	u32 ic_size;     /* field 'size' in romFS specs */
	u32 ic_checksum; /* field 'checksum' in romFS specs */
	char ic_name[0];
	//char ic_data[0];
};

/* in-memory inode filesystem entry */
struct inode {
	struct icommon *i_ic;    /* pointer to the on-device inode */
	char *i_name;            /* field 'file_name' in romFS specs */
	u8 *i_data;              /* field 'file_data' in romFS specs */
	struct list_head i_list; /* list of inodes in the filesystem */
};


/* int vfs_mount(vfsp,pathp,datap) Mount vfsp (i.e. read the superblock etc.). Pathp points to the path name */
/* 			    to be mounted (for recording purposes), and datap points to file system */
/* 	dependent data. */
/* 	vfs_unmount(vfsp) Unmount vfsp (a.e. sync the superblock). */


/* device interfaces */
//struct romfs_root *build_romfs(void *data);

/* vfs interfaces */
int romfs_mount(struct vfs *vfsp, struct vnode *mvp);

/* vnode interfaces */
int romfs_lookup(struct vnode *dvp, struct vnode **vpp, const char *name);
int romfs_open(struct vnode *vp, int flags);
int romfs_close(struct vnode *vp, int flags);
int romfs_seek(struct vnode *vp, off_t oldoff, off_t *newoffp);
int romfs_read(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n);
int romfs_getattr(struct vnode *vp, struct vattr *vap, int flags/* , cred_t *cr */);

int romfs_init(void);

#endif /* !KERNEL_FS_ROMFS_H */
