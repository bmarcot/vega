/*
 * drivers/mtd/mtdchar.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _DRIVERS_MTD_MTDCHAR_H
#define _DRIVERS_MTD_MTDCHAR_H

#define MTDCHAR_MAJOR     2
#define MTDCHAR_DEVICE_N  8

int mtdchar_init(void);

#endif /* !_DRIVERS_MTD_MTDCHAR_H */
