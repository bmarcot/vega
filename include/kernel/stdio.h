/*
 * include/kernel/stdio.h
 *
 * Copyright (c) 2018-2019 Benoit Marcot
 */

#ifndef _KERNEL_STDIO_H
#define _KERNEL_STDIO_H

/* formatted output conversion */
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);

int putchar(int c);
int puts(const char *s);

#endif /* !_KERNEL_STDIO_H */
