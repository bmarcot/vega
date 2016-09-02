/*
 * kernel/fs/fs.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <sys/types.h>

#include <kernel/fs/fs.h>
#include <kernel/fs/pathname.h>
#include <kernel/fs/vfs.h>
#include <kernel/fs/vnode.h>

#include "kernel.h"

static int slots = 0;    //FIXME: limit the global number of files to 32

//FIXME: the file table is shall be part of the process struct
static struct file *file_table[FD_MAX];

/* filedesc manipulation functions */

int fd_find(void)
{
	for (int i = 0; i < FD_MAX; i++) {
		if ((slots & (1 << i)) == 0)
			return i;
	}

	return -1;
}

void fd_reserve(int fd)
{
	slots |= (1 << fd);
}

void fd_release(int fd)
{
	slots &= ~(1 << fd);
}

int fd_validate(int fd)
{
	if ((fd >= 0) && (fd < FD_MAX))
		return 1;

	return 0;
}

/* virtual filesystem */

// https://www-01.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.kernextc/req_filesys_implement.htm
// https://www-01.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.ktechrf1/List.htm?lang=en

static struct vnode vn_root = {
	.v_path = "/",
	.v_type = VDIR,
	.v_head = LIST_HEAD_INIT(vn_root.v_head),
	.v_parent = NULL
};

static struct vnode vn_dev = {
	.v_path = "dev",
	.v_type = VDIR,
	.v_head = LIST_HEAD_INIT(vn_dev.v_head)
};

// clookup ?? this is not part of the user interface (i.e. not a syscall)
int lookup_1(struct vnode *dvp, struct vnode **vpp, const char *name)
/* int vnop_lookup(struct vnode *dvp, struct vnode **vpp, char *name, */
/* 		/\* struct vattr *vattrp, struct ucred *crp *\/) */
{
	printk("(fs) in lookup\n");

	/* 1) Lookup in the vnode cache first (or, dentry cache in Linux).
	 * 2) Then walk the list of vnodes for that vfs mounted device.
	 * 3) Then search the device.  */

	return VOP_LOOKUP(dvp, vpp, name);
}

/* copen - common open in Solaris */
int open_1(const char *pathname, int flags)
{
	int fd;
	struct vnode *vp;

	/* find or create the vnode for that pathname, and open the file on device */

	if (pathname[0] == '/') {
		lookuppn(&vn_root, &vp, pathname);
		//FIXME: Could not find the file?
	} else {
		//lookuppn(get_cur_dir(), &vp, pathname);
		printk("Relative pathnames are not supported, supply an absolute pathname instead.\n");
		return -1;
	}

	if (VOP_OPEN(vp, flags)) {
		printk("(fs) VOP_OPEN failed\n");
		return -1;
	}
	printk("(fs) VOP_OPEN success\n");

	/* find and reserve a file descriptor */
	fd = fd_find();
	if (fd < 0) {
		printk("(fs) can't find a free fd\n");
		VOP_CLOSE(vp, 0);
		return -1;
	}
	fd_reserve(fd);

	/* stitch together file structure, file descriptor, and vnode */
	struct file *file;
	file = malloc(sizeof(struct file));
	if (file == NULL) { //FIXME: use kmem_cache
		fd_release(fd);
		VOP_CLOSE(vp, 0);
		return -1;
	}
	file->f_vnode = vp;
	file->f_pos = 0;
	file_table[fd] = file;

	return fd;
}

ssize_t read_1(int fd, void *buf, size_t count)
{
	size_t n;
	struct file *file;

	if (!fd_validate(fd))
		return -1;
	file = file_table[fd];
	if (file == NULL)
		return -1;
	VOP_READ(file->f_vnode, buf, count, file->f_pos, &n);
	file->f_pos += n;

	return n;
}

off_t seek_1(int fd, off_t offset, int whence)
{
	struct file *file = file_table[fd];
	struct vattr vap;

	// seek is handled in struct file, the VFS function just check the validity of bounds..
	// VFS_SEEK = inherently, this function tests if file is seekable

	VOP_GETATTR(file->f_vnode, &vap, 0);  /* get the file size */
	printk("(fs) seek(): file size on inode is %d bytes.\n", vap.va_size);
	printk("(fs) whence = %d, offset=%d\n", whence, offset);

	switch (whence) {
	case SEEK_SET:
		if (offset)
			offset = vap.va_size % offset;
		break;
	case SEEK_CUR:
		offset = (file->f_pos + offset) % vap.va_size;
		break;
	case SEEK_END:
		offset = (vap.va_size + offset) % vap.va_size;
		break;
	default:
		return -1;
	}

	/* is the file seekable? is the offset valid? */
	if (VOP_SEEK(file->f_vnode, file->f_pos, &offset) < 0)
		return -1;

	/* move the position pointer */
	file->f_pos = offset;

	return 0;
}

int stat_1(const char *pathname, struct stat *buf)
{
	struct vnode *vp;
	struct vattr vap;

	if (pathname[0] == '/') {
		lookuppn(&vn_root, &vp, pathname);
	} else {
		//lookuppn(get_cur_dir(), &vp, pathname);
		// see read() function
		;
	}
	VOP_GETATTR(vp, &vap, 0);
	buf->st_size = vap.va_size;

	return 0;
}

/* Linux protoype */
int mount_1(const char *source, const char *target, const char *filesystemtype,
	unsigned long mountflags, const void *data)
{
	(void)source;
	(void)mountflags;

	struct vfsdef *vfsdefp = vfsdef_find(filesystemtype);

	if (vfsdefp == NULL) {
		printk("(fs) can 't find VFS\n");
		return -1;
	}
	printk("(fs) Found VFS %s\n", filesystemtype);

	/* is device initialized? if not, call device init function */
	/* if (!is_dev_initialized(source)) */
	/* 	dev_init(source); */

	/* create the VFS struct according to fstype */
	struct vfs *vfsp = malloc(sizeof(struct vfs));
	if (vfsp == NULL)
		return -1;
	vfsp->vfs_ops = vfsdefp->vfsops;
	vfsp->vfs_data = (void *)data;

	/* create the mounted-over vnode with target pathname */
	struct vnode *mvp = vn_alloc();
	if (mvp == NULL)
		return -1;
	mvp->v_path = (char *)target;
	//mvp->v_data = (void *) data;

	/* call the filesystem-dependent mount function */
	VFS_MOUNT(vfsp, mvp);

	list_add(&mvp->v_list, &vn_root.v_head);

	return 0;
}

extern struct vnode vn_dev_rand;
extern struct vnode vn_dev_zero;

void fs_init(void)
{
	vn_insert(&vn_dev, &vn_root);
	vn_insert(&vn_dev_rand, &vn_dev);
	vn_insert(&vn_dev_zero, &vn_dev);
}

/*
 fstab
 ---------------------------------
   name    type     data addr (superblock)
   sda1    romfs    xxxx_xxxxh
   ...
*/

/* module_load(romfs)
     romfs_init()
       vfsdef_register()

   mount(/dev/sda1)
     struct mntdev = { .type = flashdrive, .data = xxxx_xxxxh };
     dev_init(/dev/sda1, mntdev)
     VFS_MOUNT(...)
*/

/* some tools: ls, touch, rm.. */

static void print_offset(int offset)
{
	for (int u = 0; u < offset - 1; u++)
		printk("    ");
}

/* Recursive function! Run this in a thread or in a coroutine. */
static int ls_aux(struct vnode *vn, int offset)
{
	struct vnode *vp;

	if (offset) {
		print_offset(offset);
		printk("|-- ");
	}
	printk("%s\n", vn->v_path);
	if (vn->v_type == VDIR) {
		list_for_each_entry(vp, &vn->v_head, v_list) {
			ls_aux(vp, offset + 1);
		}
	}

	return 0;
}

void ls(void)
{
	ls_aux(&vn_root, 0);
}

