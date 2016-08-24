/*
 * kernel/irq.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#define IRQ_MAX  64

void (*irq_handler[IRQ_MAX])(void);

void irq_attach(unsigned int irq, void (*handler)(void))
{
	if (irq < IRQ_MAX)
		irq_handler[irq] = handler;
}

/* void irq_enable(int irq) */
/* { */
/* } */
