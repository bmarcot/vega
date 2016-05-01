#ifndef KERNEL_H
#define KERNEL_H

/* GCC weak symbol declaration */
#define __weak __attribute__((weak))

/* Placeholder, will be used to gather init function in the same local memory
 *  area, and mitigate cache contention.    */
#define __init

/* Placeholder, such qualified pointers should be checked at runtime. */
#define __user

int printk(const char *fmt, ...);

#endif /* !KERNEL_H */
