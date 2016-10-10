/*
 * drivers/char/mem.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <string.h>

#include <kernel/fs/vnode.h>

int mem_open(struct vnode *vp, int flags)
{
	(void)vp;
	(void)flags;

	return 0;
}

int read_zero(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;
	(void)off;
	(void)n;

	memset(buf, 0, count);
	*n = count;

	return 0;
}

static const struct vnodeops zero_vops = {
	.vop_open = mem_open,
	.vop_read = read_zero,
};

struct memdev {
	const char *name;
	const struct vnodeops *vops;
};

struct memdev devlist[] = {
	{ "zero", &zero_vops },
};

static struct vnode memvns[8];

extern struct vnode vn_dev;

void devfs_mem_init(void)
{
	//FIXME: replace with device_create_with_prealloc()
	for (int i = 0; i < 1; i++) {
		memvns[i].v_path = (char *)devlist[i].name;
		memvns[i].v_type = VCHR;
		INIT_LIST_HEAD(&memvns[i].v_head);
		memvns[i].v_ops = devlist[i].vops;
		vn_insert(&memvns[i], &vn_dev);
	}
}
