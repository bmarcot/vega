/*
 * kernel/irq.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/kernel.h>
#include <kernel/types.h>

#define IRQ_MAX  64

void (*irq_handler[IRQ_MAX])(void);

void irq_attach(unsigned int irq, void (*handler)(void))
{
#ifdef DEBUG
	printk("Attach IRQ % 2d to <%p>\n", irq, (u32)handler & ~1);
#endif
	if (irq < IRQ_MAX)
		irq_handler[irq] = handler;
}

/* void irq_enable(int irq) */
/* { */
/* } */
