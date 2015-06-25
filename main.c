#include <stdio.h>

#include "uart.h"
#include "systick.h"

extern void *vector_base;
extern void set_vtor(void *);

void start_kernel(void)
{
    uart_enable();

    printf("the k2 microkernel.\n");

    set_vtor(&vector_base);

    systick_init(0x227811);
    systick_enable();

    for (;;)
	;
}
