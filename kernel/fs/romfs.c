/*
 * kernel/fs/romfs.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

/* Design document:
 *   https://www.kernel.org/doc/Documentation/filesystems/romfs.txt  */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "linux/list.h"
#include "utils.h"

/* #include <arpa/inet.h> */
#include "inet.h" // words are big endian in romfs

#include <kernel/fs/fs.h>
#include <kernel/fs/romfs.h>
#include <kernel/fs/vfs.h>
#include <kernel/fs/vnode.h>

#include "kernel.h"

static const struct vnodeops romfs_vops = {
	.vop_open = romfs_open,
	.vop_lookup = romfs_lookup,
	.vop_read = romfs_read,
	.vop_seek = romfs_seek,
	.vop_getattr = romfs_getattr
};

static const struct vfsops romfs_vfsops = {
	.vfs_mount = romfs_mount
};

/* int romfs_init(void *data) */
int romfs_init(void)
{
	printk("(fs) Registering a romFS module\n");
	if (vfsdef_register("romfs", &romfs_vfsops))
		return -1;

	return 0;
}

// device init
//   `- romfs_dev_init
//      `- flashdrive_init
static struct romfs_root *build_romfs(void *data)
{
	struct romfs_root *nfo;

	nfo = malloc(sizeof(struct romfs_root));
	if (nfo == NULL)
		return NULL;

	nfo->r_superblock = data;

	nfo->r_volume = ((struct superblock *)data)->volume_name;
	printk("volume name = %s\n", nfo->r_volume);
	INIT_LIST_HEAD(&nfo->r_inodes);

	/* first file header comes after volume_name field */
	unsigned long filehdr_off = offsetof(struct superblock, volume_name) +
		align_next(strlen(nfo->r_volume), 16);

	/* do nothing as the supplied filesystem is empty */
	//XXX: it should be OK to build an empty filesystem..
	if (filehdr_off == nfo->r_superblock->full_size)
		return NULL;

	struct icommon *icommonp = (struct icommon *)((char *)data + filehdr_off);
	for (int i = 0; i < MAX_FILES_PER_DEV; i++) {
		//struct romfs_inode *filehdr = &file_headers[i++]; /* use kmem_cache instead */
		struct inode *filehdr = malloc(sizeof(struct inode)); /* use kmem_cache instead */
		if (filehdr == NULL)
			return NULL;
		filehdr->i_ic = icommonp;
		filehdr->i_name = icommonp->ic_name;
		printk("found filename %s (size=%dB)\n", filehdr->i_name, icommonp->ic_size);
		filehdr->i_data = (u8 *)icommonp->ic_name + align_next(strlen(filehdr->i_name), 16);
		list_add(&filehdr->i_list, &nfo->r_inodes);
		u32 next_filehdr = ntohl(icommonp->ic_next) & ~0xf; //XXX: why this mask??? USe a SHIFT instead!!!
		if (!next_filehdr)
			break;
		icommonp = (struct icommon *)((char *)data + next_filehdr);
	}

	return nfo;
}

/* in the current implementation we build the list of files during mount.
   it is actually better to build that list at lookup time, because the
   list of files may be potentially big and we don\t want to spend time
   building that list during bootstrap as we want to keep the boot time
   minimal.    */
int romfs_mount(struct vfs *vfsp, struct vnode *mvp)
{
	// romfs_root in memory struct has been created previously. How?? as a device??
	struct romfs_root *fs_root;
	struct inode *ip;
	struct vnode *vp;

	fs_root = build_romfs(vfsp->vfs_data);
	if (fs_root == NULL)
		return -1;

	/* do the early checks, is it a romFS filesystem? */
	if (strncmp((char *)fs_root->r_superblock->magic_number, "-rom1fs-", 8))
		return -1;

	/* overwrite the raw filesystem data with a romfs_root structure */
	vfsp->vfs_data = fs_root;

	mvp->v_type = VDIR;
	mvp->v_parent = NULL; // or rootfs?
	mvp->v_vfsp = vfsp;
	mvp->v_ops = &romfs_vops;
	mvp->v_count++;
	INIT_LIST_HEAD(&mvp->v_head);

	list_for_each_entry(ip, &fs_root->r_inodes, i_list) {
		vp = vn_alloc();
		if (vp == NULL)
			return -1;
		vp->v_data = ip;
		vp->v_path = ip->i_name;
		vp->v_vfsp = mvp->v_vfsp;
		vp->v_parent = mvp;    /* romFS is a flat filesystem */
		vp->v_ops = &romfs_vops;
		vp->v_type = VREG; //FIXME: flat fs at the moment, but look at the spec_info field in the inode
		INIT_LIST_HEAD(&vp->v_head);
		list_add(&vp->v_list, &mvp->v_head);
	}

	return 0;
}

/* struct vattr * vattrp; */
/* struct ucred * crp; */
int romfs_lookup(struct vnode *dvp, struct vnode **vpp, const char *name)
{
	struct vnode *vp;

	printk("(fs) in romFS lookup\n");

	*vpp = NULL;

	/* The romFS is a flat filesystem: if the file exists, then it is in the
	 * list of the mounted-over vnode.  */
	list_for_each_entry(vp, &dvp->v_head, v_list) {
		if (!strcmp(vp->v_path, name)) {
			*vpp = vp;
			printk("(fs) found the vnode\n");
			break;
		}
	}

	/* Lookup the file on the device if it's not in the list of vnodes of
	 * the mounted node.  */
	//FIXME: At the moment all vnode/inode are created at system bootstrap.

	if (*vpp == NULL)
		return -1;

	printk("(fs) romfs_lookup returns 0\n");
	return 0;
}


int romfs_open(struct vnode *vp, int flags)
{
	(void)flags;

	if (vp == NULL)
		return -1;
	vp->v_count++;
	//FIXME: if open for write and wlock already acquired, return error

	return 0;
}

#include <sys/cdefs.h>

int romfs_close(struct vnode *vp, __unused int flags)
{
	if (vp == NULL)
		return -1;
	vp->v_count--;

	/* release the vnode */
	if (!vp->v_count)
		// free the vnode?
		free(vp);

	return 0;
}


int romfs_seek(struct vnode *vp, off_t oldoff, off_t *newoffp)
{
	/* The Flash device is fully mapped into the address-space, hence there
	   is no need to shift the device's physical cursor.    */

	(void)vp;
	(void)oldoff;
	(void)newoffp;

	return 0;
}


int romfs_read(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	struct inode *ip = vp->v_data;

	if (off + count >= ip->i_ic->ic_size)
		count = ip->i_ic->ic_size - off;
	memcpy(buf, &ip->i_data[off], count);
	*n = count;

	return 0;
}


/* Gets the attributes for the supplied vnode.    */
int romfs_getattr(struct vnode *vp, struct vattr *vap, __unused int flags/* , cred_t *cr */)
{
	struct inode *ip = vp->v_data;

	vap->va_size = ntohl(ip->i_ic->ic_size);

	return 0;
}