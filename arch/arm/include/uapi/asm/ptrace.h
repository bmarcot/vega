/*
 * arch/arm/include/uapi/asm/ptrace.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _ASM_PTRACE_H
#define _ASM_PTRACE_H

#include <kernel/types.h>

/*
 * This struct defines the way the registers are stored on the stack during an
 * exception. Note that sizeof(struct pt_regs) has to be a multiple of 8 (for
 * stack alignment).
 */
struct pt_regs {
	/* preserved registers on interrupt entry */
	u32	r4;
	u32	r5;
	u32	r6;
	u32	r7;
	u32	r8;
	u32	r9;
	u32	r10;
	u32	r11;

	/* cpu-saved registers */
	u32	r0;
	u32	r1;
	u32	r2;
	u32	r3;
	u32	r12;
	u32	lr;
	u32	pc;		/* ReturnAddress */
	u32	status;		/* xPSR */
};

#endif /* !_ASM_PTRACE_H */
