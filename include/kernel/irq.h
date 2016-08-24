/*
 * include/kernel/irq.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H

void irq_attach(unsigned int irq, void (*handler)(void));

#endif /* !KERNEL_IRQ_H */
