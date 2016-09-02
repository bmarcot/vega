/*
 * kernel/fs/dev-zero.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/fs/vnode.h>

int dev_zero_open(struct vnode *vp, int flags)
{
	(void)vp;
	(void)flags;

	return 0;
}

int dev_zero_read(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;
	(void)off;
	(void)n;

	for (size_t u = 0; u < count; u++)
		((char *)buf)[u] = 0;

	return 0;
}

static const struct vnodeops dev_zero_vops = {
	.vop_open = dev_zero_open,
	.vop_read = dev_zero_read,
};

struct vnode vn_dev_zero = {
	.v_path = "zero",
	.v_type = VCHR,
	.v_head = LIST_HEAD_INIT(vn_dev_zero.v_head),
	.v_ops = &dev_zero_vops
};
