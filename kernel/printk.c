/*
 * kernel/printk.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdarg.h>

#include <kernel/types.h>

#define VSNPRINTF_BUF_SIZE 256

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
void __printk_putchar(char c);

int printk(const char *format, ...)
{
	static char buf[VSNPRINTF_BUF_SIZE];
	va_list ap;
	int count = 0;

	va_start(ap, format);
	if (vsnprintf(buf, VSNPRINTF_BUF_SIZE, format, ap) < 0)
		return -1;
	for (char *c = buf; *c != '\0'; c++, count++)
		__printk_putchar(*c);
	va_end(ap);

	return count;
}
