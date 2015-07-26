
	@ context synchronization
	.macro	sync
	dsb	sy
	isb	sy
	.endm	/* sync */

	@ movl	rd, imm32
	.macro	movl r, v
	movw	\r, #(\v & 0xffff)
	movt	\r, #(\v >> 16)
	.endm	/* movl */
