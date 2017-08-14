/*
 * kernel/spi.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <string.h>

#include <kernel/device.h>
#include <kernel/mm.h>
#include <kernel/spi.h>
#include <kernel/stddef.h>

int spi_setup(struct spi_device *spi)
{
	if (spi->max_speed_hz < spi->master->min_speed_hz)
		return -1;

	return spi->master->setup(spi);
}

int spi_transfer(struct spi_device *spi, struct spi_message *mesg)
{
	return spi->master->transfer(spi, mesg);
}

struct spi_master *spi_alloc_master(struct device *dev, unsigned size)
{
	struct spi_master *master;

	if (!dev)
		return NULL;

	master = kzalloc(size + sizeof(struct spi_master));
	if (!master)
		return NULL;

	spi_master_set_devdata(master, &master[1]);

	return master;
}

struct spi_device *spi_new_device(struct spi_master *master,
				struct spi_board_info *chip)
{
	struct spi_device *spi;

	spi = kzalloc(sizeof(struct spi_device));
	if (!spi)
		return NULL;

	spi->master = master;
	spi->chip_select = chip->chip_select;
	spi->max_speed_hz = chip->max_speed_hz;
	spi->mode = chip->mode;
	spi->controller_data = chip->controller_data;
	spi->controller_state = NULL;

	//FIXME: Attach the CS gpio pin from a different source,
	// from the master->cs_gpios array?
	spi->cs_gpio = chip->cs_gpio;

	if (spi_setup(spi))
		kfree(spi);

	return spi;
}

int spi_sync(struct spi_device *spi, struct spi_message *mesg)
{
	struct spi_master *master = spi->master;

	mesg->spi = spi;

	return master->transfer_one_message(master, mesg);
}

int spi_write_then_read(struct spi_device *spi,
			const void *tx_buf, unsigned tx_len,
			void *rx_buf, unsigned rx_len)
{
	struct spi_message message;
	struct spi_transfer x[2];

	spi_message_init(&message);
	memset(x, 0, sizeof(x));
	if (tx_len) {
		x[0].tx_buf = tx_buf;
		x[0].len = tx_len;
		spi_message_add_tail(&x[0], &message);
	}
	if (rx_len) {
		x[1].rx_buf = rx_buf;
		x[1].len = rx_len;
		spi_message_add_tail(&x[1], &message);
	}

	return spi_sync(spi, &message);
}

int spi_write(struct spi_device *spi, const void *buf, size_t len)
{
	struct spi_message message;
	struct spi_transfer x;

	spi_message_init(&message);
	memset(&x, 0, sizeof(x));
	x.tx_buf = buf;
	x.len= len;

	return spi_sync(spi, &message);
}

int spi_read(struct spi_device *spi, void *buf, size_t len)
{
	struct spi_message message;
	struct spi_transfer x;

	spi_message_init(&message);
	memset(&x, 0, sizeof(x));
	x.rx_buf = buf;
	x.len= len;

	return spi_sync(spi, &message);
}

int spi_sync_transfer(struct spi_device *spi, struct spi_transfer *xfers,
		unsigned int num_xfers)
{
	struct spi_message message;

	spi_message_init(&message);
	for (unsigned int i = 0; i < num_xfers; ++i)
		spi_message_add_tail(&xfers[i], &message);

	return spi_sync(spi, &message);
}
