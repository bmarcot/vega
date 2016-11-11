/*
 * drivers/mtd/mtdchar.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/device.h>
#include <kernel/fs/vnode.h>

#include <drivers/mtd/mtd.h>

//note: romfs will use these functions

int mtdchar_open(struct vnode *vn, int flags)
{
	(void)vn, (void)flags;

	return 0;
}

int mtdchar_read(struct vnode *vn, void *buf, size_t count, off_t off, size_t *n)
{
	struct device *dev = vn->v_data;
	struct mtd_info *mtd = dev->drvdata;

	return mtd_read(mtd, off, count, n, buf);
}

int mtdchar_write(struct vnode *vn, void *buf, size_t count, off_t off, size_t *n)
{
	struct device *dev = vn->v_data;
	struct mtd_info *mtd = dev->drvdata;

	return mtd_write(mtd, off, count, n, buf);
}

int mtdchar_getattr(struct vnode *vn, struct vattr *vap, int flags)
{
	(void) flags;

	struct device *dev = vn->v_data;
	struct mtd_info *mtd = dev->drvdata;

	vap->va_size = mtd->size;

	return 0;
}

int mtdchar_seek(struct vnode *vn, off_t oldoff, off_t *newoffp)
{
	(void)vn, (void)oldoff, (void)newoffp;

	return 0;
}

static const struct vnodeops mtdchar_vops = {
	.vop_open = mtdchar_open,
	.vop_read = mtdchar_read,
	.vop_write = mtdchar_write,
	.vop_getattr = mtdchar_getattr,
	.vop_seek = mtdchar_seek,
};

struct cdev mtdchar_cdev = {
	.cdev_vops = &mtdchar_vops,
};
