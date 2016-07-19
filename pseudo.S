/* pseudo.S
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifdef __ASSEMBLER__

	@ movl	rd, imm32
	.macro	movl rd, imm32
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	.if 	\imm32 & 0xffffff00
	ldr	\rd, =\imm32
	.else
	movs	\rd, \imm32
	.endif
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	movw	\rd, #(\imm32 & 0xffff)
	.if	\imm32 & 0xffff0000
	movt	\rd, #(\imm32 >> 16)
	.endif
#endif
	.endm

#endif
