#include <stdint.h>
#include <unistd.h>
#include "kernel.h"
#include "uart.h"
#include "linux/stddef.h"
#include "sys/cdefs.h"

int _isatty(__unused int file)
{
	return 1;
}

int _open(__unused const char *path, __unused int flags, ...)
{
	return -1;
}

int _close(__unused int file)
{
	return -1;
}

int _fstat(/* __unused int file, __unused struct stat *st */)
{
	return -1;
}

int _lseek(__unused int file, __unused int ptr, __unused int dir)
{
	return -1;
}

int _read(__unused int file, __unused char *ptr, __unused int len)
{
	return -1;
}

#define STDOUT_FILENO 1

int _write(int file, char *ptr, int len)
{
	int i;

	if (file == STDOUT_FILENO) {
		for (i = 0; i < len; i++) {
			uart_putchar(*ptr);
			if (*ptr++ == '\n')
				uart_putchar('\r');
		}
	}

	return len;
}

void *_sbrk(intptr_t incr)
{
	(void) incr;

	return NULL;
}

void _exit(__unused int status)
{
	while (1)
		;
}

typedef int clock_t;
clock_t _times(/* __unused struct tms *buf */)
{
	return -1;
}
