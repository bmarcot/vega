/*
 * kernel/printk.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdarg.h>
#include <stdio.h>
#include <ucontext.h>

#define VSNPRINTF_BUF_SIZE 256

static ucontext_t printk_context;
static ucontext_t vsnprintf_context = { .uc_link = &printk_context };
static unsigned int ctx_stack[128];
static char vsnprintf_buf[VSNPRINTF_BUF_SIZE];
static int retval;

/* this coroutine is not thread-safe, not reentrant */
void co_vsnprintf(const char *format, va_list ap)
{
	retval = vsnprintf(vsnprintf_buf, VSNPRINTF_BUF_SIZE, format, ap);
}

void __printk_putchar(char c);

int printk(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsnprintf_context.uc_stack.ss_sp = &ctx_stack[128];
	makecontext(&vsnprintf_context, co_vsnprintf, 2, format, ap);
	swapcontext(&printk_context, &vsnprintf_context);
	for (char *c = vsnprintf_buf; *c != '\0'; c++)
		__printk_putchar(*c);
	va_end(ap);

	return retval;
}
