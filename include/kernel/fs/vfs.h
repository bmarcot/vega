/*
 * include/kernel/fs/vfs.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_FS_VFS_H
#define KERNEL_FS_VFS_H

#include <kernel/fs/vnode.h>

#include "linux/list.h"

/* Figure: SunOS 5.x VFS Operations Interface */

struct vfsops {
	int (*vfs_mount)();
	/* int (*vfs_unmount)(); */
	/* int (*vfs_root)(); */
	/* int (*vfs_statvfs)(); */
	/* int (*vfs_sync)(); */
	/* int (*vfs_vget)(); */
	/* int (*vfs_mountroot)(); */
	/* int (*vfs_swapvp)(); */
};

/* https://www.filesystems.org/docs/zadok-thesis-proposal/node79.html */

struct vfs {
	struct vfs *vfs_next;
	int vfs_fstype;
	const struct vfsops *vfs_ops;
	void *vfs_data;
};

/* int fsop_mount(vfs_t *vfsp, vnode_t *mvp, struct mounta *uap, cred_t *cr) */

#define VFS_MOUNT(vfsp, mvp)			\
	(vfsp->vfs_ops->vfs_mount(vfsp, mvp))

static inline
int vfs_mount(struct vfs *vfsp, struct vnode *mvp)
{
	return vfsp->vfs_ops->vfs_mount(vfsp, mvp);
}

/* #define VFS_UNMOUNT(vfsp, cr)         (*(vfsp)->vfs_op->vfs_unmount)(vfsp, cr) */
/* #define VFS_ROOT(vfsp, vpp)           (*(vfsp)->vfs_op->vfs_root)(vfsp, vpp) */
/* #define VFS_STATVFS(vfsp, sp)         (*(vfsp)->vfs_op->vfs_statvfs)(vfsp, sp) */
/* #define VFS_SYNC(vfsp, flag, cr)      (*(vfsp)->vfs_op->vfs_sync)(vfsp, flag, cr) */
/* #define VFS_VGET(vfsp, vpp, fidp)     (*(vfsp)->vfs_op->vfs_vget)(vfsp, vpp, fidp) */
/* #define VFS_MOUNTROOT(vfsp, init)     (*(vfsp)->vfs_op->vfs_mountroot)(vfsp, init) */
/* #define VFS_SWAPVP(vfsp, vpp, nm)     (*(vfsp)->vfs_op->vfs_swapvp)(vfsp, vpp, nm) */

/* virtual filesystem */

/* details from:
 *
 *   https://www-01.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.kernextc/imp_data_struct_filesys_impl.htm%23a29c010f14
*/
/* struct vfs { */
/* 	/\* Contains the state flags: */
/* 	 * */
/* 	 *   VFS_DEVMOUNT */
/* 	 *     Indicates whether the virtual file system has a physical mount */
/* 	 *     structure underlying it. */
/* 	 * */
/* 	 *   VFS_READONLY */
/* 	 *     Indicates whether the virtual file system is mounted read-only. */
/* 	 *\/ */
/* 	int v_flag; */

/* 	/\* Identifies the type of file system implementation. Possible values */
/* 	 * for this field are described in the /usr/include/sys/vmount.h file. */
/* 	 *\/ */
/* 	int v_type; */

/* 	/\* Points to the set of operations for the specified file system type. *\/ */
/* 	int v_ops; */

/* 	/\* Points to the mounted-over v-node. *\/ */
/* 	int v_mntdover; */

/* 	/\* Points to the file system implementation data. The interpretation of */
/* 	 * this field is left to the discretion of the file system implementation. */
/* 	 * For example, the field could be used to point to data in the kernel */
/* 	 * extension segment or as an offset to another segment. */
/* 	 *\/ */
/* 	void *v_data; */

/* 	/\* Records the user arguments to the mount call that created this virtual */
/* 	 * file system. This field has a time stamp. The user arguments are */
/* 	 * retained to implement the mntctl call, which replaces the /etc/mnttab */
/* 	 * table. */
/* 	 *\/ */
/* 	int v_mdata; */
/* }; */

struct vfsdef {
	const char *name;
	const struct vfsops *vfsops;
	//FIXME: Add am init function?
	struct list_head list;
};

int vfsdef_register(const char *name, const struct vfsops *vfsops);
int vfsdef_deregister(const char *name);
struct vfsdef *vfsdef_find(const char *name);

#endif /* !KERNEL_FS_VFS_H */
