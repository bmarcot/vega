#include <stdio.h>

#include "uart.h"
#include "systick.h"
#include "page.h"

extern void *vector_base;
extern void set_vtor(void *);

int start_kernel(void)
{
    uart_enable();

    printf("the k2 microkernel v%d.%d\n", 0, 1);

    page_alloc();
    page_alloc();
    page_alloc();
    page_alloc();

    set_vtor(&vector_base);

    systick_init(0x227811);
    systick_enable();

    return 0;
}

void cpu_locked(int errno)
{
    printf("%d: cpu locked\n", errno);
    for (;;)
    	;
}

void cpu_idle(void)
{
    printf("cpu idle\n");
    for (;;)
	;
}
