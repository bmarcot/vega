/*
 * include/kernel/v7m-helper.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_V7M_HELPER_H
#define KERNEL_V7M_HELPER_H

static inline void *v7m_set_thumb_bit(void *addr)
{
    return (void *)((unsigned long)addr | 1ul);
}

static inline void *v7m_clear_thumb_bit(void *addr)
{
    return (void *)((unsigned long)addr & ~1ul);
}

#endif /* !KERNEL_V7M_HELPER_H */
