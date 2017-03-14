/*
 * platform/nrf52/spim.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stddef.h>

#include <kernel/types.h>

#include "platform.h"

struct spim_config config = {
	.pin_sck = 10,
	.pin_mosi = 11,
	.pin_miso = 12,
	.freq = SPIM_FREQUENCY_FREQUENCY_M8,
};

struct spim_config curr_config;

void nrf52_spim_init(struct spim_config *config)
{
	memcpy(&curr_config, config, sizeof(struct spim_config));

	/* disable the peripheral */
	NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos;

	/* configure SCK pin */
	nrf52_gpio_direction_output(config->pin_sck, 0); /* must be same as CONFIG.CPOL */
	NRF_SPIM0->PSEL.SCK = (config->pin_sck << SPIM_PSEL_SCK_PIN_Pos)
		| (SPIM_PSEL_SCK_CONNECT_Connected << SPIM_PSEL_SCK_CONNECT_Pos);

	/* configure MOSI pin */
	nrf52_gpio_direction_output(config->pin_mosi, 0);
	NRF_SPIM0->PSEL.MOSI = (config->pin_mosi << SPIM_PSEL_MOSI_PIN_Pos)
		| (SPIM_PSEL_MOSI_CONNECT_Connected << SPIM_PSEL_MOSI_CONNECT_Pos);

	/* configure MISO pin */
	nrf52_gpio_direction_input(config->pin_miso);
	NRF_SPIM0->PSEL.MISO = (config->pin_miso << SPIM_PSEL_MISO_PIN_Pos)
		| (SPIM_PSEL_MISO_CONNECT_Connected << SPIM_PSEL_MISO_CONNECT_Pos);

	/* set SPI frequency */
	NRF_SPIM0->FREQUENCY = config->freq << SPIM_FREQUENCY_FREQUENCY_Pos;

	/* use SPI Mode 0 */
	NRF_SPIM0->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos)
		| (SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos)
		| (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);

	/* enable the peripheral */
	NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;
}

void spi_transfer(const void *tx_buf, size_t tx_len, void *rx_buf,
		size_t rx_len, u8 overread_char)
{
	NRF_SPIM0->ORC = overread_char;
	NRF_SPIM0->TXD.PTR = (unsigned int)tx_buf;
	NRF_SPIM0->TXD.MAXCNT = tx_len;

	if (rx_buf == NULL) {
		NRF_SPIM0->RXD.MAXCNT = 0;
	} else {
		NRF_SPIM0->RXD.PTR = (unsigned int)rx_buf;
		NRF_SPIM0->RXD.MAXCNT = rx_len;
	}

	NRF_SPIM0->EVENTS_END = 0;
	NRF_SPIM0->TASKS_START = 1;
	while (!NRF_SPIM0->EVENTS_END)
		;
}

void nrf52_spim_cs_low(void)
{
	nrf52_gpio_direction_output(curr_config.pin_cs, 0);
}

void nrf52_spim_cs_up(void)
{
	nrf52_gpio_direction_output(curr_config.pin_cs, 1);
}
