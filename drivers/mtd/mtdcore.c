/*
 * drivers/mtd/mtdcore.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stddef.h>

#include <kernel/bitops.h>
#include <kernel/device.h>

#include <drivers/mtd/mtd.h>
#include <drivers/mtd/mtdchar.h>

void mtd_erase_callback(struct erase_info *instr)
{
	if (instr->callback)
		instr->callback(instr);
}

int mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	if ((instr->addr >= mtd->size) || (instr->len > mtd->size - instr->addr))
		return -EINVAL;
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	instr->fail_addr = MTD_FAIL_ADDR_UNKNOWN;
	if (!instr->len) {
		instr->state = MTD_ERASE_DONE;
		mtd_erase_callback(instr);
		return 0;
	}

	return mtd->mtd_erase(mtd, instr);
}

int mtd_point(struct mtd_info *mtd, off_t from, size_t len, size_t *retlen,
	void **virt/* , resource_size_t *phys */)
{
	*retlen = 0;
	*virt = NULL;
	if (!mtd->mtd_point)
		return -1;//return -EOPNOTSUPP;
	if ((from < 0) || ((unsigned long)from >= mtd->size) || (len > mtd->size - from))
		return -EINVAL;
	if (!len)
		return 0;

	return mtd->mtd_point(mtd, from, len, retlen, virt/* , phys */);
}

int mtd_read(struct mtd_info *mtd, off_t from, size_t len, size_t *retlen,
	unsigned char *buf)
{
	*retlen = 0;
	if ((from < 0) || ((unsigned long)from >= mtd->size) || (len > mtd->size - from))
		return -EINVAL;
	if (!len)
		return 0;

	return mtd->mtd_read(mtd, from, len, retlen, buf);
}

int mtd_write(struct mtd_info *mtd, off_t to, size_t len, size_t *retlen,
	const unsigned char *buf)
{
	*retlen = 0;
	if ((to < 0) || ((unsigned long)to >= mtd->size) || (len > mtd->size - to))
		return -EINVAL;
	/* if (!mtd->mtd_write || !(mtd->flags & MTD_WRITEABLE)) { */
	/* 	printk("EROFS\n"); */
	/* 	return -EROFS; */
	/* } */
	if (!len)
		return 0;

	return mtd->mtd_write(mtd, to, len, retlen, buf);
}

/* By default, all MTD device are mtdchar devices, and can be r/w with
 * the char device interface. Alternatively, the filesystem located on
 * the device can be mounted. */
int add_mtd_device(struct mtd_info *mtd, const char *name)
{
	static unsigned long free_map = 0;
	unsigned long bit;

	bit = find_first_zero_bit(&free_map, MTDCHAR_DEVICE_N);
	if (bit == MTDCHAR_DEVICE_N)
		return -1;
	set_bit(bit, &free_map);
	chrdev_add(MKDEV(MTDCHAR_MAJOR, bit), name);

	struct device *dev = device_alloc();
	if (dev == NULL)
		return -1;
	dev_set_devt(dev, MKDEV(MTDCHAR_MAJOR, bit));
	dev_set_drvdata(dev, mtd);
	device_add(dev);

	return 0;
}

struct mtd_info *get_mtd_device(dev_t devt)
{
	struct device *dev = device_get(devt);

	return dev_get_drvdata(dev);
}
