/*
 * kernel/fs/dev-rand.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/fs/vnode.h>

int dev_rand_init(void)
{
	/* init device, driver, seed, hal... */

	return 0;
}

int dev_rand_open(struct vnode *vp, int flags)
{
	(void)vp;
	(void)flags;

	return 0;
}

int dev_rand_close(struct vnode *vp, int flags)
{
	(void)vp;
	(void)flags;

	return 0;
}

//FIXME: this pseudo-random algorithm is really bad :)
extern int main();
int dev_rand_read(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;
	(void)off;
	(void)n;

	static u8 seed = 97;
	u8 *ent = (u8 *) main;

	for (unsigned u = 0; u < count; u++) {
		((u8 *) buf)[u] = (*(ent + u) * 251) ^ seed;
		seed++;
	}

	return 0;
}

static const struct vnodeops dev_rand_vops = {
	.vop_open = dev_rand_open,
	.vop_read = dev_rand_read,
};

struct vnode vn_dev_rand = {
	.v_path = "random",
	.v_type = VCHR,
	.v_head = LIST_HEAD_INIT(vn_dev_rand.v_head),
	.v_ops = &dev_rand_vops
};

/* ------------ libc ------------ */

/* int rand(void); */
/* { */
/* 	int r; */

/* 	int dev_fd = open("/dev/random"); */
/* 	read_1(dev_fd, &r, sizeof (int)); */
//close(dev_fd);

/* 	return r; */
/* } */

