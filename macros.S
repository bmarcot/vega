
	@ context synchronization
	.macro	sync
	dsb	sy
	isb	sy
	.endm

	@ movl	rd, imm32
	.macro	movl rd, imm32
	movw	\rd, #(\imm32 & 0xffff)
	movt	\rd, #(\imm32 >> 16)
	.endm
