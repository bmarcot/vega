/*
 * drivers/mtd/mtdram.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <string.h>
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

/* static struct mtd_info mtdram = { */
/* 	.dev  = call create_decice() -- /dev/mtd0 */
/* 	.priv = 0 + user_offset */
/* }; */

#include "kernel.h"
#include <kernel/device.h>

extern struct cdev mtdchar_cdev;

static struct device dev = {
	.char_dev = &mtdchar_cdev,
};

struct mtd_info mtdram;

extern char __mtdram_start__;
extern char __mtdram_size__;

void mtdram_init(void)
{
	mtdram_init_device(&mtdram, &__mtdram_start__,
			(unsigned long)&__mtdram_size__, "mtd0");
	printk("Inited mtd device %s\n", mtdram.name);
	dev.drvdata = &mtdram;
	mkdev(&dev, "mtd0");
}