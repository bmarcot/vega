/*
 * platform/nrf52/twim.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <stddef.h>

#include <kernel/types.h>

#include "platform.h"

#define SCL_PIN_CONF ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
			| (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) \
			| (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos) \
			| (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) \
			| (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos))
#define SDA_PIN_CONF SCL_PIN_CONF

void nrf52_twim_init(struct twim_config *config)
{
	/* disable the peripheral */
	NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;

	/* configure SCL pin */
	NRF_P0->PIN_CNF[config->pin_scl] = SCL_PIN_CONF;
	NRF_TWIM0->PSEL.SCL = (config->pin_scl << TWIM_PSEL_SCL_PIN_Pos)
		| (TWIM_PSEL_SCL_CONNECT_Connected << TWIM_PSEL_SCL_CONNECT_Pos);

	/* configure SDA pin */
	NRF_P0->PIN_CNF[config->pin_sda] = SDA_PIN_CONF;
	NRF_TWIM0->PSEL.SDA = (config->pin_sda << TWIM_PSEL_SDA_PIN_Pos)
		| (TWIM_PSEL_SDA_CONNECT_Connected << TWIM_PSEL_SDA_CONNECT_Pos);

	NRF_TWIM0->FREQUENCY = config->freq << TWIM_FREQUENCY_FREQUENCY_Pos;
	NRF_TWIM0->ADDRESS = config->address;
	/* NRF_TWIM0->SHORTS = */
	/* 	TWIM_SHORTS_LASTRX_STOP_Enabled << TWIM_SHORTS_LASTRX_STOP_Pos; */

	/* enable the peripheral */
	NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
}

void nrf52_twim_disable(void)
{
	/* disable the peripheral */
	NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
}

int i2c_read(volatile unsigned char *rx_buf, size_t rx_len, uint8_t addr)
{
	volatile uint8_t tx_buf[1];

	NRF_TWIM0->SHORTS = TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk;

	tx_buf[0] = addr;
	NRF_TWIM0->TXD.MAXCNT = 1;
	NRF_TWIM0->TXD.PTR = (uint32_t)&tx_buf[0];

	NRF_TWIM0->RXD.MAXCNT = rx_len;
	NRF_TWIM0->RXD.PTR = (uint32_t)rx_buf;

	NRF_TWIM0->EVENTS_STOPPED = 0;
	NRF_TWIM0->TASKS_STARTTX = 1;
	while (NRF_TWIM0->EVENTS_STOPPED == 0)
		;

	return NRF_TWIM0->RXD.AMOUNT;
}

void i2c_write(uint8_t addr, uint8_t data)
{
	volatile uint8_t tx_buf[2];

	NRF_TWIM0->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;

	tx_buf[0] = addr;
	tx_buf[1] = data;
	NRF_TWIM0->TXD.MAXCNT = sizeof(tx_buf);
	NRF_TWIM0->TXD.PTR = (uint32_t)&tx_buf[0];

	NRF_TWIM0->EVENTS_STOPPED = 0;
	NRF_TWIM0->TASKS_STARTTX = 1;
	while (NRF_TWIM0->EVENTS_STOPPED == 0)
		;
}
