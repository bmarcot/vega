#ifndef KERNEL_H
#define KERNEL_H

/* GCC weak symbol declaration */
#define __weak __attribute__((weak))

int printk(const char *fmt, ...);

#endif /* !KERNEL_H */
