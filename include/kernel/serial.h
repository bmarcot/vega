/*
 * include/kernel/serial.h
 *
 * Copyright (c) 2016 Benoit Marcot
 *
 * Interface for serial devices (UART, ...)
 */

#ifndef _KERNEL_SERIAL_H
#define _KERNEL_SERIAL_H

#include <sys/types.h>

struct device;

struct serial_info {
	unsigned int rx_count;

	int (*serial_init)(struct serial_info *serial);

	int (*serial_getc)(struct serial_info *serial, char *c);
	int (*serial_gets)(struct serial_info *serial, size_t len,
			size_t *retlen, char *buf);
	int (*serial_putc)(struct serial_info *serial, char c);
	int (*serial_puts)(struct serial_info *serial, size_t len,
			size_t *retlen, const char *buf);

	/* callback on device activity, set by ioctl() */
	void (*callback)(struct serial_info *self);

	struct device *dev;
	void *priv;
};

int serial_getc(struct serial_info *serial, char *c);
int serial_gets(struct serial_info *serial, size_t len, size_t *retlen,
		char *buf);
int serial_putc(struct serial_info *serial, char c);
int serial_puts(struct serial_info *serial, size_t len, size_t *retlen,
		const char *buf);

void serial_activity_callback(struct serial_info *serial);

#endif /* !_KERNEL_SERIAL_H */
