#ifndef UART_H
#define UART_H

struct uart {
    unsigned int uartdr;
    char pad[0x28];
    unsigned int uartlcrh;
    unsigned int uartctl;
};

void uart_enable(void);
void uart_putchar(char);
void uart_putstring(const char *);

#endif /* !UART_H */
