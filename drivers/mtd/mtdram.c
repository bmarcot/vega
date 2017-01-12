/*
 * drivers/mtd/mtdram.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include <drivers/mtd/mtd.h>

#define SIZE_1KB 1024

static int mtdram_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	memset((char *)mtd->priv + instr->addr, 0xff, instr->len);
	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;
}

static int mtdram_point(struct mtd_info *mtd, off_t from, size_t len,
			size_t *retlen, void **virt/* , resource_size_t *phys */) // void **at
{
	*virt = mtd->priv + from;
	*retlen = len;

	return 0;
}

static int mtdram_unpoint(struct mtd_info *mtd, off_t from, size_t len)
{
	(void)mtd, (void)from, (void)len;

	return 0;
}

int mtdram_write(struct mtd_info *mtd, off_t to, size_t len,
			size_t *retlen, const void *buf)
{
	memcpy((char *)mtd->priv + to, buf, len);
	*retlen = len;

	return 0;
}

int mtdram_read(struct mtd_info *mtd, off_t from, size_t len,
		size_t *retlen, void *buf)
{
	memcpy(buf, mtd->priv + from, len);
	*retlen = len;

	return 0;
}

int mtdram_init_device(struct mtd_info *mtd, void *mapped_address,
			unsigned long size, const char *name)
{
	if (size % SIZE_1KB)
		return -1;
	mtd->name = name;
	mtd->size = size;
	mtd->erasesize = SIZE_1KB,
	mtd->priv = mapped_address;
	mtd->mtd_erase = mtdram_erase;
	mtd->mtd_point = mtdram_point;
	mtd->mtd_unpoint = mtdram_unpoint;
	mtd->mtd_read = mtdram_read;
	mtd->mtd_write = mtdram_write;

	return 0;
}

struct mtd_info mtdram;

extern char __mtdram_start__;
extern char __mtdram_size__;

extern const struct file_operations mtdchar_fops;
extern const struct inode_operations tmpfs_iops;

/* struct inode *create_dev_inode(const char *name, */
/* 			const struct file_operations *fops); */

extern struct inode tmpfs_inodes[];
extern struct dentry tmpfs_dentries[];

static struct inode mtd0_inode = {
	.i_ino     = 1200,
	.i_op      = &tmpfs_iops,
	.i_fop     = &mtdchar_fops,
	.i_private = &mtdram,
};

static struct dentry mtd0_dentry = {
	.d_inode  = &mtd0_inode,
 	.d_parent = &tmpfs_dentries[1],
	.d_name   = "mtd0",
};

void mtdram_init(void)
{
	printk("Creating MTD device %s\n", mtd0_dentry.d_name);
	mtdram_init_device(&mtdram, &__mtdram_start__,
			(unsigned long)&__mtdram_size__, mtd0_dentry.d_name);
	tmpfs_link(NULL, &tmpfs_inodes[1], &mtd0_dentry);
}
