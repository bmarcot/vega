/*
 * include/kernel/fs/vnode.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_FS_VNODE_H
#define KERNEL_FS_VNODE_H

#include <sys/types.h>

#include "linux/list.h"

/* SunOS 5.x Vnode Operations Interface */
//FIXME: take the latest Solaris interface instead

struct vnodeops {
	int (*vop_open)();
	int (*vop_close)();
	int (*vop_read)();
	int (*vop_write)();
	int (*vop_lookup)();
	int (*vop_seek)();
	int (*vop_getattr)();
};

/* Solaris 10 vnode Types from sys/vnode.h */
enum vtype {
	VNON,   /* No type */
	VREG,   /* Regular file */
	VDIR,   /* Directory */
	VBLK,   /* Block device */
	VCHR,   /* Character device */
	VLNK,   /* Symbolic link */
	VFIFO,  /* Named pipe */
	VDOOR,  /* Doors interface */
	VPROC,  /* procfs node */
	VSOCK,  /* sockfs node (socket) */
	VPORT,  /* Event port */
	VBAD    /* Bad vnode */
};

/* AIX: V-nodes are used only to translate a path name into a generic node (g-node). */
/* a v-node links a filename (a directory entry) to an i-node
 *   http://www.solarisinternals.com/si/sample/solaris_internals_ch14_file_system_framework.pdf  */
struct vnode {
	//kmutex_t v_lock; /* protects vnode fields */
	char *v_path; /* cached path */
	const struct vnodeops *v_ops; /* vnode operations */
	int v_count; /* reference count */
	struct vfs *v_vfsp; /* ptr to containing VFS */
	void *v_data; /* private data for fs */ //FIXME: is it the inode??
	enum vtype v_type;

	struct list_head v_list; /* list of vnodes in that directory */
	struct list_head v_head; /* list of vnodes pointing to this this vnode */
	struct vnode *v_parent; /* pointer to parent vnode */
};

struct vattr {
	off_t va_size;
};

/* vnode manipulations */

struct vnode *vnode_alloc(void);
void vnode_free(struct vnode *vn);
int vnode_reinit(struct vnode *vn);

//XXX: Is it a helper function?
int vn_insert(struct vnode *vp, struct vnode *vp_head);

/* vnode operations */

#define VOP_LOOKUP(dvp, vpp, name) \
	fop_lookup(dvp, vpp, name)

#define VOP_OPEN(vp, flags) \
	fop_open(vp, flags)

#define VOP_CLOSE(vp, flags) \
	fop_close(vp, flags)

#define VOP_SEEK(vp, oldoff, newoffp) \
	fop_seek(vp, oldoff, newoffp)

#define VOP_READ(vp, buf, count, off, n) \
	fop_read(vp, buf, count, off, n)

#define VOP_GETATTR(vp, vap, flags) \
	fop_getattr(vp, vap, flags)

static inline
int fop_lookup(struct vnode *dvp, struct vnode **vpp, const char *name)
{
	return dvp->v_ops->vop_lookup(dvp, vpp, name);
}

static inline
int fop_open(struct vnode *vp, int flags)
{
	return vp->v_ops->vop_open(vp, flags);
}

static inline
int fop_close(struct vnode *vp, int flags)
{
	return vp->v_ops->vop_close(vp, flags);
}

static inline
int fop_seek(struct vnode *vp, off_t oldoff, off_t *newoffp)
{
	return vp->v_ops->vop_seek(vp, oldoff, newoffp);
}

static inline
int fop_read(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	return vp->v_ops->vop_read(vp, buf, count, off, n);
}

static inline
int fop_getattr(struct vnode *vp, struct vattr *vap, int flags/* , cred_t *cr */)
{
	return vp->v_ops->vop_getattr(vp, vap, flags);
}

/* http://www.manualpages.de/OpenBSD/OpenBSD-5.0/man9/VOP_WRITE.9.html */

#define VOP_WRITE(vp, buf, count, off, n) \
	fop_write(vp, buf, count, off, n)

static inline
int fop_write(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	return vp->v_ops->vop_write(vp, buf, count, off, n);
}

#endif /* !KERNEL_FS_VNODE_H */
