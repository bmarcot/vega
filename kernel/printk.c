/*
 * kernel/printk.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/ktime.h>
#include <kernel/sched_clock.h>
#include <kernel/stdarg.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/types.h>

#define VSNPRINTF_BUF_SIZE 256

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int snprintf(char *str, size_t size, const char *format, ...);
void __printk_putchar(char c);

extern int sched_clock_registered;

int printk(const char *format, ...)
{
	static char buf[VSNPRINTF_BUF_SIZE];
	va_list ap;

	if (sched_clock_registered) {
		struct timespec ts = ktime_to_timespec(sched_clock());
		const char fmt[] = "[% 4d.%06d] ";
		snprintf(buf, VSNPRINTF_BUF_SIZE, fmt, (int)ts.tv_sec,
			(int)ts.tv_nsec / 1000);
		puts(buf);
	}

	va_start(ap, format);
	if (vsnprintf(buf, VSNPRINTF_BUF_SIZE, format, ap) < 0)
		return -1;
	puts(buf);
	va_end(ap);

	return strlen(buf);
}
