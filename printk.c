#include <stdio.h> /* for vsnprintf() */
#include <stdarg.h>
#include "uart.h"
#include "libc/ucontext.h"

#define PRINTK_BUF_SIZE 512

static char printk_buf[PRINTK_BUF_SIZE];
static ucontext_t main_context, print_context;
static unsigned int ctx_stack[128];

/* this function is not thread-safe, nor it is reentrant */
int printk_1(const char *format, va_list ap)
{
	int printed;

	printed = vsnprintf(printk_buf, PRINTK_BUF_SIZE, format, ap);
	uart_putstring(printk_buf);

	return printed;
}

int printk(const char *format, ...)
{
	va_list ap;

	/* link the current context to the print context */
	print_context.uc_link = &main_context;
	print_context.uc_stack.ss_sp = &ctx_stack[128];

	/* pass 4 arguments to the new context, and swap */
	va_start(ap, format);
	makecontext(&print_context, printk_1, 2, format, ap);
	swapcontext(&main_context, &print_context);
	va_end(ap);

	return 0;
}
