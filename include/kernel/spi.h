/*
 * include/kernel/spi.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_SPI_H
#define _KERNEL_SPI_H

#include <string.h>

#include <kernel/device.h>
#include <kernel/list.h>
#include <kernel/stddef.h>
#include <kernel/types.h>

#define SPI_CPHA      0x01
#define SPI_CPOL      0x02
#define SPI_MODE_0    (0|0)
#define SPI_MODE_1    (0|SPI_CPHA)
#define SPI_MODE_2    (SPI_CPOL|0)
#define SPI_MODE_3    (SPI_CPOL|SPI_CPHA)
#define SPI_CS_HIGH   0x04
#define SPI_LSB_FIRST 0x08
#define SPI_3WIRE     0x10
#define SPI_LOOP      0x20
#define SPI_NO_CS     0x40
#define SPI_READY     0x80
#define SPI_TX_DUAL   0x100
#define SPI_TX_QUAD   0x200
#define SPI_RX_DUAL   0x400
#define SPI_RX_QUAD   0x800

struct spi_device {
	struct device     dev;
	struct spi_master *master;

	u32	max_speed_hz;
	u8	chip_select;
	u16	mode;
	void	*controller_data;
	void	*controller_state;
	int	cs_gpio;
};

struct spi_board_info {
	void	*controller_data;
	u32	max_speed_hz;
	u16	bus_num;
	u16	chip_select;
	u16	mode;
};

struct spi_message;

struct spi_master {
	struct device    dev;
	struct list_head list;

	s16	bus_num;
	u32	min_speed_hz;
	u32	max_speed_hz;

	int (*setup) (struct spi_device *spi);
	int (*transfer) (struct spi_device *spi, struct spi_message *mesg);
	int (*transfer_one_message) (struct spi_master *master, struct spi_message *mesg);
};

int spi_setup(struct spi_device *spi);
int spi_transfer(struct spi_device *spi, struct spi_message *mesg);
int spi_sync(struct spi_device *spi, struct spi_message *mesg);
struct spi_master *spi_alloc_master(struct device *dev, unsigned size);

static inline void spi_master_set_devdata(struct spi_master *master, void *data)
{
	dev_set_drvdata(&master->dev, data);
}

static inline void *spi_master_get_devdata(struct spi_master *master)
{
	return dev_get_drvdata(&master->dev);
}

static inline struct spi_master *to_spi_master(struct device *dev)
{
	return dev ? container_of(dev, struct spi_master, dev) : NULL;
}

struct spi_transfer {
	const void	*tx_buf;
	void		*rx_buf;
	unsigned	len;

	unsigned	cs_change:1;
	u16		delay_usecs;

	struct list_head transfer_list;
};

struct spi_message {
	struct list_head  transfers;
	struct spi_device *spi;
	int               status;

	void (*complete)(void *context);
	void *context;
};

static inline void spi_message_init_no_memset(struct spi_message *m)
{
	INIT_LIST_HEAD(&m->transfers);
}

static inline void spi_message_init(struct spi_message *m)
{
	memset(m, 0, sizeof *m);
	spi_message_init_no_memset(m);
}

static inline void
spi_message_add_tail(struct spi_transfer *t, struct spi_message *m)
{
	list_add_tail(&t->transfer_list, &m->transfers);
}

static inline void spi_transfer_del(struct spi_transfer *t)
{
	list_del(&t->transfer_list);
}

int spi_write_then_read(struct spi_device *spi,
			const void *tx_buf, unsigned tx_len,
			void *rx_buf, unsigned rx_len);

static inline ssize_t spi_w8r8(struct spi_device *spi, u8 cmd)
{
	ssize_t status;
	u8 result;

	status = spi_write_then_read(spi, &cmd, 1, &result, 1);

	/* return negative errno or unsigned value */
	return (status < 0) ? status : result;
}

static inline ssize_t spi_w8r16(struct spi_device *spi, u8 cmd)
{
	ssize_t status;
	u16 result;

	status = spi_write_then_read(spi, &cmd, 1, &result, 2);

	/* return negative errno or unsigned value */
	return (status < 0) ? status : result;
}

#endif /* !_KERNEL_SPI_H */
