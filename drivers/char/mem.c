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

int write_mem(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;

	//FIXME: Add checks, valid_phys_addr_range()
	memcpy((void *)off, buf, count);
	*n = count;

	return 0;
}

int read_mem(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;

	//FIXME: Add checks, valid_phys_addr_range()
	memcpy(buf, (void *)off, count);
	*n = count;

	return 0;
}

int write_null(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;
	(void)buf;
	(void)off;

	*n = count;

	return 0;
}

int read_null(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;
	(void)buf;
	(void)count;
	(void)off;

	*n = 0;

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

static const struct vnodeops mem_vops = {
	.vop_open = mem_open,
	.vop_read = read_mem,
	.vop_write = write_mem,
};

static const struct vnodeops null_vops = {
	.vop_open = mem_open,
	.vop_read = read_null,
	.vop_write = write_null,
};

static const struct vnodeops zero_vops = {
	.vop_open = mem_open,
	.vop_read = read_zero,
	.vop_write = write_null,
};

struct memdev {
	const char *name;
	const struct vnodeops *vops;
};

extern struct vnodeops random_vops;

struct memdev devlist[] = {
	{ "mem", &mem_vops },
	{ "null", &null_vops },
	{ "zero", &zero_vops },
	{ "random", &random_vops },
};

static struct vnode memvns[8];

extern struct vnode vn_dev;

void devfs_mem_init(void)
{
	//FIXME: replace with device_create_with_prealloc()
	for (int i = 0; i < 4; i++) {
		memvns[i].v_path = (char *)devlist[i].name;
		memvns[i].v_type = VCHR;
		INIT_LIST_HEAD(&memvns[i].v_head);
		memvns[i].v_ops = devlist[i].vops;
		vn_insert(&memvns[i], &vn_dev);
	}
}
