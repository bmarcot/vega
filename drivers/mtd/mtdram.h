/*
 * drivers/mtd/mtdram.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _DRIVERS_MTD_MTDRAM_H
#define _DRIVERS_MTD_MTDRAM_H

int mtdram_init_device(struct mtd_info *mtd, void *mapped_address,
		unsigned long size, const char *name);

#endif /* !_DRIVERS_MTD_MTDRAM_H */
