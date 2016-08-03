#include "kernel.h"
#include "uart.h"
#include "cmsis/nrf52/nrf.h"

static inline void tx_char(char c)
{
	NRF_UART0->TXD = c;
	while (NRF_UART0->EVENTS_TXDRDY == 0)
		;
	NRF_UART0->EVENTS_TXDRDY = 0;
}

void uart_putchar(char c)
{
	NRF_UARTE0->TASKS_STARTTX = 1;
	tx_char(c);
	NRF_UARTE0->TASKS_STOPTX = 1;
}

void uart_putstring(const char *s)
{
	NRF_UARTE0->TASKS_STARTTX = 1;
	while (*s) {
		if ('\n' == *s)
			tx_char('\r');
		tx_char(*s++);
	}
	NRF_UARTE0->TASKS_STOPTX = 1;
}

void __init uart_init(void)
{
	/* disable the module while (re)configuring */
	NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos;

	/* 115200bps, no flow-control, no parity */
	NRF_UART0->CONFIG = (UART_CONFIG_HWFC_Disabled   << UART_CONFIG_HWFC_Pos)
		| (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);
	NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200 << UART_BAUDRATE_BAUDRATE_Pos;

	/* use nRF52 dev kit's default pins for UART function */
	NRF_UART0->PSELTXD = 6;
	NRF_UART0->PSELRXD = 8;

	/* enable data tx and rx */
	NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;
}
