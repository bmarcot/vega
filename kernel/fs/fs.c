/*
 * kernel/fs/fs.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/fs/fs.h>
#include <kernel/fs/pathname.h>
#include <kernel/fs/vfs.h>
#include <kernel/fs/vnode.h>

#include "kernel.h"

//FIXME: the file table is part of the task structure
static struct file *file_table[FD_MAX];

static unsigned long fd_bitmap = 0; /* 32 files */

static int find_free_fd(void)
{
	unsigned long bit;

	bit = find_first_zero_bit(&fd_bitmap, BITS_PER_LONG);
	if (bit == BITS_PER_LONG)
		return -1;

	return 0;
}

static void reserve_fd(int fd)
{
	bitmap_set_bit(&fd_bitmap, fd);
}

static void release_fd(int fd)
{
	bitmap_clear_bit(&fd_bitmap, fd);
}

static int validate_fd(int fd)
{
	if ((fd >= 0) && (fd < FD_MAX) && bitmap_get_bit(&fd_bitmap, fd))
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

/* static */ struct vnode vn_dev = {
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

int sys_open(const char *pathname, int flags)
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
	fd = find_free_fd();
	if (fd < 0) {
		printk("(fs) can't find a free fd\n");
		VOP_CLOSE(vp, 0);
		return -1;
	}
	reserve_fd(fd);

	/* stitch together file structure, file descriptor, and vnode */
	struct file *file;
	file = malloc(sizeof(struct file));
	if (file == NULL) { //FIXME: use kmem_cache
		release_fd(fd);
		VOP_CLOSE(vp, 0);
		errno = ENOMEM;
		return -1;
	}
	file->f_vnode = vp;
	file->f_pos = 0;
	file_table[fd] = file;

	return fd;
}

ssize_t sys_read(int fd, void *buf, size_t count)
{
	size_t n;
	struct file *file;

	if (!validate_fd(fd)) {
		errno = EBADF;
		return -1;
	}
	file = file_table[fd];
	VOP_READ(file->f_vnode, buf, count, file->f_pos, &n);
	file->f_pos += n;

	return n;
}

ssize_t sys_write(int fd, void *buf, size_t count)
{
	size_t n;
	struct file *file;

	if (!validate_fd(fd)) {
		errno = EBADF;
		return -1;
	}
	file = file_table[fd];
	VOP_WRITE(file->f_vnode, buf, count, file->f_pos, &n);
	file->f_pos += n;

	return n;
}

off_t sys_seek(int fd, off_t offset, int whence)
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

int sys_stat(const char *pathname, struct stat *buf)
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

int mkdir(const char *pathname);

/* Linux protoype */
int sys_mount(const char *source, const char *target, const char *filesystemtype,
	unsigned long mountflags, const void *data)
{
	(void)source;
	(void)mountflags;

	struct vfsdef *vfsdefp = vfsdef_find(filesystemtype);
	struct vnode *mvp;

	if (vfsdefp == NULL) {
	  printk("error: fs: filesystemtype not configured in the kernel\n");
		errno = ENODEV;
		return -1;
	}
	printk("(fs) Found VFS %s\n", filesystemtype);

	/* is device initialized? if not, call device init function */
	/* if (!is_dev_initialized(source)) */
	/* 	dev_init(source); */

	/* create the VFS struct according to fstype */
	struct vfs *vfsp = malloc(sizeof(struct vfs));
	if (vfsp == NULL) {
		errno = ENOMEM;
		return -1;
	}
	vfsp->vfs_ops = vfsdefp->vfsops;
	vfsp->vfs_data = (void *)data;

	/* allocate the mounted-over vnode, with target pathname */
	if (mkdir(target))
		return -1;
	lookuppn(&vn_root, &mvp, target);

	/* call the filesystem-dependent mount function */
	VFS_MOUNT(vfsp, mvp);

	return 0;
}

extern void devfs_mem_init(void);

void init_filesystem(void)
{
	vnode_attach(&vn_dev, &vn_root);
	devfs_mem_init();
	printk("$ ls\n");
	ls();
	printk("\n");
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

/* fstools -- ls, touch, rm, mkdir.. */

#include <string.h>

/* Recursive function! Careful with the stack, run from thread
 * with a big enough stack, or from coroutine. */
static int ls_aux(struct vnode *vn, char *pathbuf)
{
	int pathlen;
	struct vnode *vp;

	if (strlen(pathbuf))
		printk("%s\n", pathbuf);
	else
		printk("/\n");
	if (vn->v_type == VDIR) {
		list_for_each_entry(vp, &vn->v_head, v_list) {
			pathlen = strlen(pathbuf);
			ls_aux(vp, strcat(strcat(pathbuf, "/"),	vp->v_path));
			pathbuf[pathlen] = '\0';
		}
	}

	return 0;
}

#define PATH_MAX 128

void ls(void)
{
	char pathbuf[PATH_MAX] = {[0] = 0};

	ls_aux(&vn_root, pathbuf);
}

#include <errno.h>

char *dirname(char *path);
char *basename(char *path);

int mkdir(const char *pathname/* , mode_t mode */)
{
	/* (void)mode; */

	struct vnode *vn;
	struct vnode *parent_vn;
	char *dname;

	vn = vnode_alloc();
	if (!vn) {
		errno = ENOMEM;
		return -1;
	}
	vn->v_type = VDIR;
	vn->v_path = basename((char *)pathname);

	dname = dirname((char *)pathname);
	if (!strlen(dname)) {
		//FIXME: lookuppn() must handle '/' path
		parent_vn = &vn_root;
	} else {
		//lookuppn(fsroot(), &parent_vn, dirpath);
		lookuppn(&vn_root, &parent_vn, dname);
	}
	vnode_attach(vn, parent_vn);
	/* free(dname); */

	return 0;
}
