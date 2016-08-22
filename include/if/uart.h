/* include/if/uart.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef IF_UART_H
#define IF_UART_H

void __uart_init(void);
void __uart_enable(void);
void __uart_putchar(char c);
void __uart_putstring(const char *s);

static inline void uart_init(void)
{
	__uart_init();
}

static inline void uart_enable(void)
{
	__uart_enable();
}

static inline void uart_putchar(char c)
{
	__uart_putchar(c);
}

static inline void uart_putstring(const char *s)
{
	__uart_putstring(s);
}

#endif /* !IF_UART_H */
