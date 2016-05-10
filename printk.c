#include <stdio.h> /* for vsnprintf() */
#include <stdarg.h>
#include "uart.h"

/* FIXME: There is definitely something wrong with vsnprintf function, why
   does it needs such a big buffer?!    */
#define PRINTK_BUF_SIZE 2048

static char printk_buf[PRINTK_BUF_SIZE];

/* this function is not thread-safe, nor it is reentrant */
int printk(const char *fmt, ...)
{
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsnprintf(printk_buf, PRINTK_BUF_SIZE, fmt, args);
	va_end(args);

	uart_putstring(printk_buf);

	return printed;

}
