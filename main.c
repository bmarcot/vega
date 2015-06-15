#include "uart.h"

void start_kernel(void)
{
    uart_enable();
    uart_putstring("the k2 microkernel.\n");

    for (;;)
	;
}
