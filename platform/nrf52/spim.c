/*
 * platform/nrf52/spim.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdint.h>

#include <kernel/errno-base.h>
#include <kernel/mm.h>
#include <kernel/spi.h>

#include "platform.h"

struct nrf_spim_ctrlr_state {
	uint32_t config;
	uint32_t frequency;
};

struct nrf_spim_platform_data {
	s16	bus_num;
	u32	max_speed_hz;
	u32	min_speed_hz;
	u8	pin_mosi;
	u8	pin_miso;
	u8	pin_sck;
};

static NRF_SPIM_Type *get_spim_instance(s16 bus_num)
{
	NRF_SPIM_Type *const inst[] = { NRF_SPIM0, NRF_SPIM1, NRF_SPIM2 };

	if (bus_num >  SPIM_COUNT)
		return NULL;
	return inst[bus_num];
}

static uint32_t get_max_freq(u32 master_max_speed_hz, u32 spi_max_speed_hz)
{
	if (master_max_speed_hz <= spi_max_speed_hz)
		return SPIM_FREQUENCY_FREQUENCY_M8 << SPIM_FREQUENCY_FREQUENCY_Pos;

	/* use a lookup table for frequencies in range 1MHz to 8MHz */
	const uint32_t lookup_table_mhz[] = {
		SPIM_FREQUENCY_FREQUENCY_M1 << SPIM_FREQUENCY_FREQUENCY_Pos,
		SPIM_FREQUENCY_FREQUENCY_M2 << SPIM_FREQUENCY_FREQUENCY_Pos,
		SPIM_FREQUENCY_FREQUENCY_M2 << SPIM_FREQUENCY_FREQUENCY_Pos,
		SPIM_FREQUENCY_FREQUENCY_M4 << SPIM_FREQUENCY_FREQUENCY_Pos,
		SPIM_FREQUENCY_FREQUENCY_M4 << SPIM_FREQUENCY_FREQUENCY_Pos,
		SPIM_FREQUENCY_FREQUENCY_M4 << SPIM_FREQUENCY_FREQUENCY_Pos,
		SPIM_FREQUENCY_FREQUENCY_M4 << SPIM_FREQUENCY_FREQUENCY_Pos,
		SPIM_FREQUENCY_FREQUENCY_M8 << SPIM_FREQUENCY_FREQUENCY_Pos,
	};
	u32 dev_max_speed_mhz = spi_max_speed_hz / 1000000;
	if (dev_max_speed_mhz > 0)
		return lookup_table_mhz[dev_max_speed_mhz - 1];

	if (spi_max_speed_hz >= 500000)
		return SPIM_FREQUENCY_FREQUENCY_K500 << SPIM_FREQUENCY_FREQUENCY_Pos;

	if (spi_max_speed_hz >= 250000)
		return SPIM_FREQUENCY_FREQUENCY_K250 << SPIM_FREQUENCY_FREQUENCY_Pos;

	return SPIM_FREQUENCY_FREQUENCY_K125 << SPIM_FREQUENCY_FREQUENCY_Pos;
}

static uint32_t get_config(u16 mode)
{
	const uint32_t lookup_table_pol[] = {
		/* SPI_MODE0 0 (Leading)  0 (Active High) */
		(SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos)
		| (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos),

		/* SPI_MODE1 0 (Leading)  1 (Active Low) */
		(SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos)
		| (SPIM_CONFIG_CPOL_ActiveLow << SPIM_CONFIG_CPOL_Pos),

		/* SPI_MODE2 1 (Trailing) 0 (Active High) */
		(SPIM_CONFIG_CPHA_Trailing << SPIM_CONFIG_CPHA_Pos)
		| (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos),

		/* SPI_MODE3 1 (Trailing) 1 (Active Low) */
		(SPIM_CONFIG_CPHA_Trailing << SPIM_CONFIG_CPHA_Pos)
		| (SPIM_CONFIG_CPOL_ActiveLow << SPIM_CONFIG_CPOL_Pos),
	};

#define SPI_MODE_MSK 0x3

	uint32_t config = lookup_table_pol[mode & SPI_MODE_MSK];

	if (mode & SPI_LSB_FIRST)
		config |= SPIM_CONFIG_ORDER_LsbFirst << SPIM_CONFIG_ORDER_Pos;
	else
		config |= SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos;

	return config;
}

int nrf_spim_setup(struct spi_device *spi)
{
	struct nrf_spim_ctrlr_state *state = kmalloc(sizeof(*state));
	if (!state)
		return -1;

	state->config = get_config(spi->mode);
	state->frequency = get_max_freq(spi->master->max_speed_hz,
					spi->max_speed_hz);
	spi->controller_state = state;

	return 0;
}

int nrf52_gpio_direction_output(int, int);
int nrf52_gpio_direction_input(int);

int nrf_spim_transfer_one_message(struct spi_master *master,
				struct spi_message *mesg)
{
	struct spi_device *spi = mesg->spi;
	NRF_SPIM_Type *spim = get_spim_instance(master->bus_num);

	/* reconfigure and enable the master */
	struct nrf_spim_ctrlr_state *state = spi->controller_state;
	spim->CONFIG = state->config;
	spim->FREQUENCY = state->frequency;
	spim->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;

	int chip_select = !(spi->mode & SPI_NO_CS);
	int selected = 0;
	struct spi_transfer *xfer;
	list_for_each_entry(xfer, &mesg->transfers, transfer_list) {
		if (chip_select && !selected) {
			nrf52_gpio_direction_output(spi->cs_gpio,
						!!(spi->mode & SPI_CS_HIGH));
			selected = 1;
		}

		/* spim->ORC = transfer->overread_char; */
		if (xfer->tx_buf) {
			spim->TXD.PTR = (uint32_t)xfer->tx_buf;
			spim->TXD.MAXCNT = xfer->len;
		} else {
			spim->TXD.MAXCNT = 0;
		}
		if (xfer->rx_buf) {
			spim->RXD.PTR = (uint32_t)xfer->rx_buf;
			spim->RXD.MAXCNT = xfer->len;
		} else {
			spim->RXD.MAXCNT = 0;
		}
		spim->EVENTS_END = 0;
		spim->TASKS_START = 1;

		//XXX: sync? async?
		while (!spim->EVENTS_END)
			;

		if (chip_select && xfer->cs_change) {
			nrf52_gpio_direction_output(spi->cs_gpio,
						!(spi->mode & SPI_CS_HIGH));
			selected = 0;
		}
		if (xfer->delay_usecs) {
			; //usleep(xfer->delay_usecs);
		}
	}

	// disable the SPIM interface and deselect the device
	spim->ENABLE = SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos;
	if (chip_select && selected)
		nrf52_gpio_direction_output(spi->cs_gpio, !(spi->mode & SPI_CS_HIGH));

	return 0;
}

/* init == probe */
int nrf_spim_init(struct device *dev)
{
	struct spi_master *master;
	struct nrf_spim_platform_data *hw = dev->platform_data;

	master = spi_alloc_master(dev, 0);
	if (!master) {
		/* dev_err(&ndev->pci_dev->dev, */
		/* 	"%s(): unable to alloc SPI master\n", __func__); */
		return -EINVAL;
	}
	master->transfer_one_message = nrf_spim_transfer_one_message;
	master->setup = nrf_spim_setup;
	master->bus_num = hw->bus_num;
	master->max_speed_hz = hw->max_speed_hz;
	master->min_speed_hz = hw->min_speed_hz;

	/* configure SCK pin */
	nrf52_gpio_direction_output(hw->pin_sck, 0); /* must be same as CONFIG.CPOL */
	NRF_SPIM0->PSEL.SCK = (hw->pin_sck << SPIM_PSEL_SCK_PIN_Pos)
		| (SPIM_PSEL_SCK_CONNECT_Connected << SPIM_PSEL_SCK_CONNECT_Pos);

	/* configure MOSI pin */
	nrf52_gpio_direction_output(hw->pin_mosi, 0);
	NRF_SPIM0->PSEL.MOSI = (hw->pin_mosi << SPIM_PSEL_MOSI_PIN_Pos)
		| (SPIM_PSEL_MOSI_CONNECT_Connected << SPIM_PSEL_MOSI_CONNECT_Pos);

	/* configure MISO pin */
	nrf52_gpio_direction_input(hw->pin_miso);
	NRF_SPIM0->PSEL.MISO = (hw->pin_miso << SPIM_PSEL_MISO_PIN_Pos)
		| (SPIM_PSEL_MISO_CONNECT_Connected << SPIM_PSEL_MISO_CONNECT_Pos);

	return 0;
}

int nrf_spim_enable(struct device *dev)
{
	struct spi_master *master = to_spi_master(dev);
	NRF_SPIM_Type *spim = get_spim_instance(master->bus_num);

	spim->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;
	dev->offline = 0;

	return 0;
}

int nrf_spim_disable(struct device *dev)
{
	struct spi_master *master = to_spi_master(dev);
	NRF_SPIM_Type *spim = get_spim_instance(master->bus_num);

	spim->ENABLE = SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos;
	dev->offline = 1;

	return 0;
}

const struct nrf_spim_platform_data nrf_spim_platform_data[] = {
	[0] = { 0, 8000000, 125000, 10, 11, 12 },
};

const struct device_driver nrf_spim_device_driver = {
	.probe = nrf_spim_init,
	//.remove = xxx; //XXX: must dealloc memory
	//.shutdown = nrf_spim_disable,
	.suspend = nrf_spim_disable,
	.resume = nrf_spim_enable,
};

int spim_init(void)
{
	struct device nrf_spim = {
		.driver = (struct device_driver *)&nrf_spim_device_driver,
	};

	for (int i = 0; i < SPIM_COUNT; i++) {
		nrf_spim.platform_data = (struct platform_data *)&nrf_spim_platform_data[i];
		nrf_spim_init(&nrf_spim);
	}

	return 0;
}
