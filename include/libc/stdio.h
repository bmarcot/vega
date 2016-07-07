#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>

int snprintf(char *str, size_t size, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);

#endif /* !STDIO_H */
