#ifndef UART_H
#define UART_H

void uart_enable(void);
void uart_putchar(char);
void uart_putstring(const char *);
void uart_init(void);

#endif /* !UART_H */
