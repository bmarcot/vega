/*
 * kernel/fs/proc.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

/* A simple procfs layer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/fs/fs.h>
#include <kernel/fs/vnode.h>

int proc_open(struct vnode *vp, int flags)
{
	(void)flags;

	vp->v_data = NULL;

	return 0;
}


int proc_close(struct vnode *vp, int flags)
{
	(void)flags;

	free(vp->v_data);

	return 0;
}

int version_proc_read(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp, (void)count, (void)off;

	char *single_buf;
	if (vp->v_data == NULL) {
		single_buf = malloc(64);
		snprintf(single_buf, 64, "Version %d.2.3 far-paper (GCC 4.%d.x)", 1, 1);
		vp->v_data = single_buf;
	} else {
		single_buf = vp->v_data;
	}
	strncpy(buf, single_buf + off, count);
	*n = strlen(buf);

	return 0;
}

extern char __pgmem_size__;
extern char __heap_size__;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;

int meminfo_proc_read(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp, (void)count, (void)off;

	char *single_buf;
	if (vp->v_data == NULL) {
		single_buf = malloc(192);
		int bss_size = &__bss_end__ - &__bss_start__;
		int data_size = &__data_end__ - &__data_start__;
		snprintf(single_buf, 192,
			"MemTotal:   % 6d Bytes\n"
			"MemFree:    % 6d Bytes\n"
			"KernelData: % 6d Bytes\n"
			"KernelZeroData: %d Bytes\n",
			64 * 1024,
			64 * 1024 - (int)&__pgmem_size__ - (int)&__heap_size__
			- bss_size - data_size,
			data_size, bss_size);
		vp->v_data = single_buf;
	} else {
		single_buf = vp->v_data;
	}
	strncpy(buf, single_buf + off, count);
	*n = strlen(buf);

	return 0;
}

static const struct vnodeops version_proc_vops = {
	.vop_open = proc_open,
	.vop_close = proc_close,
	.vop_read = version_proc_read,
};

static const struct vnodeops meminfo_proc_vops = {
	.vop_open = proc_open,
	.vop_close = proc_close,
	.vop_read = meminfo_proc_read,
};

static struct vnode vn_proc = {
	.v_path = "proc",
	.v_type = VDIR,
	.v_head = LIST_HEAD_INIT(vn_proc.v_head)
};

static struct vnode vn_version_proc = {
	.v_path = "version",
	.v_type = VCHR,
	.v_head = LIST_HEAD_INIT(vn_version_proc.v_head),
	.v_ops = &version_proc_vops,
};

static struct vnode vn_meminfo_proc = {
	.v_path = "meminfo",
	.v_type = VCHR,
	.v_head = LIST_HEAD_INIT(vn_meminfo_proc.v_head),
	.v_ops = &meminfo_proc_vops,
};

void proc_init(void)
{
	vnode_attach(&vn_proc, fsroot());
	vnode_attach(&vn_version_proc, &vn_proc);
	vnode_attach(&vn_meminfo_proc, &vn_proc);
}
