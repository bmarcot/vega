/*
 * proc-macros.S
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _ASM_ASSEMBLER_H
#define _ASM_ASSEMBLER_H

#ifdef __ASSEMBLER__

/*
 * mov32 - loads a 32-bit value into a register without a data access
 */
	.macro	mov32 rd, imm32
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	.if 	\imm32 & 0xffffff00
	ldr	\rd, =\imm32
	.else
	movs	\rd, \imm32
	.endif
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	movw	\rd, #:lower16:\imm32
	.if	\imm32 & 0xffff0000
	movt	\rd, #:upper16:\imm32
	.endif
#endif
	.endm

#endif /* __ASSEMBLER__ */

#endif /* !_ASM_ASSEMBLER_H */
