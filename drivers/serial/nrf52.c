/*
 * drivers/serial/nrf52.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <stdint.h>
#include <sys/types.h>

#include <kernel/cbuf.h>
#include <kernel/fs.h>
#include <kernel/irq.h>
#include <kernel/serial.h>

#include "platform.h"

static struct cbuf_info cbuf;
static char buf[16];

static char c;

int nrf52_getc(struct serial_info *serial, char *c)
{
	serial->rx_count--;
	cbuf_getc(&cbuf, c);

	return 0;
}

int nrf52_putc(struct serial_info *serial, char c)
{
	NRF_UARTE_Type *uarte = serial->priv;

	uarte->EVENTS_ENDTX = 0;
	uarte->EVENTS_TXSTOPPED = 0;
	uarte->TXD.MAXCNT = 1;
	uarte->TXD.PTR = (uint32_t)&c;
	uarte->TASKS_STARTTX = 1;
	while (!uarte->EVENTS_ENDTX)
		;
	uarte->TASKS_STOPTX = 1;
	while (!uarte->EVENTS_TXSTOPPED)
		;

	return 0;
}

int nrf52_puts(struct serial_info *serial, size_t len, size_t *retlen,
	const char *buf)
{
	*retlen = len;
	for (int i = 0; len > 0; len--, i++)
		nrf52_putc(serial, buf[i]);

	return 0;
}

struct serial_info nrf52_serial0 = {
	.serial_getc = nrf52_getc,
	.serial_putc = nrf52_putc,
	.serial_puts = nrf52_puts,
	.priv        = NRF_UARTE0,
};

static void nrf52_uarte0_isr(void)
{
	cbuf_putc(&cbuf, c);
	NRF_UARTE0->EVENTS_ENDRX = 0;
	nrf52_serial0.rx_count++;
	serial_activity_callback(&nrf52_serial0);
}

extern const struct file_operations serialchar_fops;
extern const struct inode_operations tmpfs_iops;

static struct inode nrf52_inode = {
	.i_ino     = 1200,
	.i_op      = &tmpfs_iops,
	.i_fop     = &serialchar_fops,
	.i_private = &nrf52_serial0,
};

int nrf52_serial_init(void)
{
	struct dentry dentry = { .d_inode = &nrf52_inode,
				 .d_name  = "ttyS0" };

	cbuf_init(&cbuf, buf, 16);
	vfs_link(0, dev_inode(), &dentry);

	/* disable the module while (re)configuring */
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;

	/* 115200bps, no flow-control, no parity */
	NRF_UARTE0->CONFIG = (UARTE_CONFIG_HWFC_Disabled   << UARTE_CONFIG_HWFC_Pos)
		| (UARTE_CONFIG_PARITY_Excluded << UARTE_CONFIG_PARITY_Pos);
	NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud115200 << UARTE_BAUDRATE_BAUDRATE_Pos;

	/* use nRF52 dev kit's default pins for UART function */
	NRF_UARTE0->PSEL.TXD = 6;
	NRF_UARTE0->PSEL.RXD = 8;

	/* configure link */
	NRF_UARTE0->RXD.PTR = (uint32_t)&c;
	NRF_UARTE0->RXD.MAXCNT = 1;
	NRF_UARTE0->SHORTS = UARTE_SHORTS_ENDRX_STARTRX_Enabled
		<< UARTE_SHORTS_ENDRX_STARTRX_Pos;
	NRF_UARTE0->INTENSET =
		UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos;
	NRF_UARTE0->EVENTS_RXSTARTED = 0;
	NRF_UARTE0->EVENTS_ENDRX = 0;

	/* enable rx interrupt */
	irq_attach(UARTE0_UART0_IRQn, nrf52_uarte0_isr);
	NVIC_EnableIRQ(UARTE0_UART0_IRQn);

	/* enable data tx and rx */
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;

	NRF_UARTE0->TASKS_STARTRX = 1;
	while (!NRF_UARTE0->EVENTS_RXSTARTED)
		;

	return 0;
}
