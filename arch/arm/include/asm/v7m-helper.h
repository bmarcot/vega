/*
 * arch/arm/include/asm/v7m-helper.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _ASM_V7M_HELPER_H
#define _ASM_V7M_HELPER_H

#define V7M_EXC_RETURN_HANDLER_MAIN   0xfffffff1
#define V7M_EXC_RETURN_THREAD_MAIN    0xfffffff9
#define V7M_EXC_RETURN_THREAD_PROCESS 0xfffffffd

/* execution privilege when running in Thread_Mode */
#define V7M_PRIVILEGED   0x0
#define V7M_UNPRIVILEGED 0x1

#ifndef __ASSEMBLER__

static inline void *v7m_set_thumb_bit(void *p)
{
    return (void *)((unsigned long)p | 1ul);
}

static inline void *v7m_clear_thumb_bit(void *p)
{
    return (void *)((unsigned long)p & ~1ul);
}

#endif /* !__ASSEMBLER__ */

#endif /* !_ASM_V7M_HELPER_H */
