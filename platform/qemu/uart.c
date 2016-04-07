#include "kernel.h"
#include "uart.h"

struct uart {
	unsigned int uartdr;
	char pad[0x28];
	unsigned int uartlcrh;
	unsigned int uartctl;
};

volatile struct uart *uart0 = (void *) 0x4000c000;

void uart_enable(void)
{
	uart0->uartctl |= 1;
	uart0->uartlcrh |= (3 << 5);
}

void uart_putchar(char c)
{
	while ((*(volatile int *) 0x4000c018) & (1 << 3))
		;
	uart0->uartdr = c;
}

void uart_putstring(const char *s)
{
	while (*s) {
		if ('\n' == *s)
			uart_putchar('\r');
		uart_putchar(*s++);
	}
}

int puts(const char *s)
{
	while (*s)
		uart_putchar(*s++);
	uart_putchar('\r');
	uart_putchar('\n');

	return 0;
}

void __init uart_init(void)
{
	uart_enable();
}
