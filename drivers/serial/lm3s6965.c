/*
 * drivers/serial/lm3s6965.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/device.h>
#include <kernel/fs/vnode.h>
#include <kernel/irq.h>
#include <kernel/serial.h>
#include <kernel/types.h>

#define LM3S_UART0_BASE       0x4000c000
#define LM3S_UARTIM_RXIM_Pos  4
#define LM3S_UARTICR_RXIC_Pos 4
#define LM3S_UART0_IRQn       5

struct lm3s6965_uart {
	u32 uartdr;      /* 0 */
	u8 pad_0[0x14];
	u32 uartfr;      /* 0x18 */
	u8 pad_1[0x10];
	u32 uartlcrh;    /* 0x2c */
	u32 uartctl;     /* 0x30 */
	u8 pad_2[4];
	u32 uartim;      /* 0x38 */
	u8 pad_3[8];
	u32 uarticr;     /* 0x44 */
};

static struct lm3s6965_uart *const uart0 =
	(struct lm3s6965_uart *)LM3S_UART0_BASE;

static char rx_cbuf[16];
static int pos_begin;
static int pos_end;

static char cbuf_getc(void)
{
	char c = rx_cbuf[pos_begin++];
	pos_begin %= 16;

	return c;
}

static void cbuf_putc(char c)
{
	rx_cbuf[pos_end++] = c;
	pos_end %= 16;
}

int lm3s6965_getc(struct serial_info *serial, char *c)
{
	serial->rx_count--;
	*c = cbuf_getc();

	return 0;
}

/* int serial_gets(struct serial_info *serial, size_t len, */
/* 		size_t *retlen, char *buf); */

int lm3s6965_putc(struct serial_info *serial, char c)
{
	struct lm3s6965_uart *uart = serial->priv;

	while (uart->uartfr & (1 << 3))
		;
	uart->uartdr = c;

	return 0;
}

int lm3s6965_puts(struct serial_info *serial, size_t len,
		size_t *retlen, const char *buf)
{
	*retlen = len;
	for (int i = 0; len > 0; len--, i++)
		lm3s6965_putc(serial, buf[i]);

	return 0;
}

struct device lm3s6965_uart0_dev;
extern struct cdev serialchar_cdev;

struct serial_info lm3s6965_uart0 = {
	.serial_getc = lm3s6965_getc,
	.serial_putc = lm3s6965_putc,
	.serial_puts = lm3s6965_puts,

	.dev = &lm3s6965_uart0_dev,
	.priv = (void *)LM3S_UART0_BASE,
};

struct device lm3s6965_uart0_dev = {
	.char_dev = &serialchar_cdev,
	.drvdata = &lm3s6965_uart0,
};

static void lm3s6965_uart0_isr(void)
{
	char c = (char)uart0->uartdr;
	cbuf_putc(c);
	lm3s6965_uart0.rx_count++;
	uart0->uarticr = 1 << LM3S_UARTICR_RXIC_Pos; /* ack */
	serial_activity_callback(&lm3s6965_uart0);
}

#include "cmsis/arm/ARMCM4.h"
#include "cmsis/arm/core_cm4.h"

int lm3s6965_init(void)
{
	mkdev(&lm3s6965_uart0_dev, "ttyS0");

	/* configure link */
	uart0->uartctl |= 1;
	uart0->uartlcrh |= (3 << 5);

	/* enable rx interrupt */
	irq_attach(LM3S_UART0_IRQn, lm3s6965_uart0_isr);
	NVIC_EnableIRQ(LM3S_UART0_IRQn);
	uart0->uartim = 1 << LM3S_UARTIM_RXIM_Pos;

	return 0;
}
