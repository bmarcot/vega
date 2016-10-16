#include <stddef.h>
#include <if/gpio.h>
#include "cmsis/nrf52/nrf.h"

NRF_SPIM_Type * spim_instance = NRF_SPIM0;

#define PIN_SCK   3
#define PIN_MOSI  12
#define PIN_MISO  11

#define SPI_MODE0_CPOL  0  /* active high */
#define SPI_MODE0_CPHA  0  /* leading */

void spim_init()
{
	printk("-- init SPIM\n");

	/* disable the SPIM */
	NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos;

	/* pins must be configured in the GPIO peripheral */
	gpio_direction_output(PIN_SCK, 0);  /* must be same as CONFIG.CPOL */
	gpio_direction_output(PIN_MOSI, 0);
	gpio_direction_input(PIN_MISO);

	/* select physical pins used by the SPI interface */
	NRF_SPIM0->PSEL.SCK = (PIN_SCK << SPIM_PSEL_SCK_PIN_Pos)
		| (SPIM_PSEL_SCK_CONNECT_Connected << SPIM_PSEL_SCK_CONNECT_Pos);
	NRF_SPIM0->PSEL.MOSI = (PIN_MOSI << SPIM_PSEL_MOSI_PIN_Pos)
		| (SPIM_PSEL_MOSI_CONNECT_Connected << SPIM_PSEL_MOSI_CONNECT_Pos);
	NRF_SPIM0->PSEL.MISO = (PIN_MISO << SPIM_PSEL_MISO_PIN_Pos)
		| (SPIM_PSEL_MISO_CONNECT_Connected << SPIM_PSEL_MISO_CONNECT_Pos);

	/* configure the SPI mode (Mode 0), and frequency (8 Mbps) */
	NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M4 << SPIM_FREQUENCY_FREQUENCY_Pos;
	NRF_SPIM0->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos)
		| (SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos)
		| (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);

	/* write 0 to MOSI when over-reading */
	NRF_SPIM0->ORC = 0;

	/* enable the SPIM */
	NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;
}

void spi_transaction(const void *tx_buf, size_t tx_len, void *rx_buf,
		size_t rx_len)
{
	NRF_SPIM0->TXD.PTR = (int)tx_buf;
	NRF_SPIM0->TXD.MAXCNT = tx_len;

	if (rx_buf == NULL) {
		NRF_SPIM0->RXD.MAXCNT = 0;
	} else {
		NRF_SPIM0->RXD.PTR = (int)rx_buf;
		NRF_SPIM0->RXD.MAXCNT = rx_len;
	}

	NRF_SPIM0->EVENTS_END = 0;
	NRF_SPIM0->TASKS_START = 1;
	while (!NRF_SPIM0->EVENTS_END)
		;
}
