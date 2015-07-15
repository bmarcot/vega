#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "uart.h"

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

int _fstat(__unused int file, __unused struct stat *st)
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

extern intptr_t heap_base;
extern intptr_t heap_limit;

void *_sbrk(intptr_t incr)
{
    static void *next = (void *) &heap_base;

    if ((next + incr) > (void *) &heap_limit)
        return (void *) 0;
    next += incr;    /* the heap just grows up */

    return next - incr;
}

void _exit(__unused int status)
{
    while (1)
	;
}

clock_t _times(__unused struct tms *buf)
{
    return -1;
}