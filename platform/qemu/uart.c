#include <if/uart.h>

struct lm3s6965_uart {
	unsigned int uartdr;
	char pad[0x28];
	unsigned int uartlcrh;
	unsigned int uartctl;
};

static volatile struct lm3s6965_uart *const uart0 = (struct lm3s6965_uart *const) 0x4000c000;

void __uart_enable(void)
{
	uart0->uartctl |= 1;
	uart0->uartlcrh |= (3 << 5);
}

void __uart_putchar(char c)
{
	while ((*(volatile int *) 0x4000c018) & (1 << 3))
		;
	uart0->uartdr = c;
}

void __uart_putstring(const char *s)
{
	while (*s) {
		if ('\n' == *s)
			__uart_putchar('\r');
		__uart_putchar(*s++);
	}
}

void __uart_init(void)
{
	__uart_enable();
}
