/*
 * arch/arm/include/asm/assembler.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _ASM_ASSEMBLER_H
#define _ASM_ASSEMBLER_H

#ifdef __ASSEMBLER__

/*
 * mov32 - loads a 32-bit value into a register without a data access
 */
	.macro	mov32 rd, imm32
#if defined(__ARM_ARCH_6M__)
	.if 	\imm32 & 0xffffff00
	 .error	"immediate value is larger than 8bits"
	.else
	movs	\rd, \imm32
	.endif
#elif defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
	movw	\rd, #:lower16:\imm32
	.if	\imm32 & 0xffff0000
	movt	\rd, #:upper16:\imm32
	.endif
#endif
	.endm

/*
 * badr - get address of symbol and set Thumb_Bit
 */
	.macro	badr rd, sym
	adr	\rd, \sym + 1
	.endm

#endif /* __ASSEMBLER__ */

#endif /* !_ASM_ASSEMBLER_H */
