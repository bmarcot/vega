
	.set	V7M_SCB_BASE, 0xe000ed00
	.set	SCB_VTOR, 8
	.set	V7M_AIRCR, 0x0c

	@ movl rd, imm32
	.macro	movl r, v
	movw	\r, #(\v & 0xffff)
	movt	\r, #(\v >> 16)
	.endm	/* movl */

	@ void	set_vtor(void *addr)
	.balign	8				//FIXME: move to linkage.h
	.global set_vtor
set_vtor:
	movl	r1, V7M_SCB_BASE
	str	r0, [r1, #SCB_VTOR]
	dsb	sy				//FIXME: in any order?
	isb	sy
	bx	lr
	.type	set_vtor, %function

	@ void	soft_reset(void)
	.balign	8
	.global soft_reset
soft_reset:
	movl	r1, V7M_SCB_BASE
	movl	r2, ((0x5fa << 16) | (1 << 2))
	dsb	sy
	str	r2, [r1, #V7M_AIRCR]
	dsb	sy
	b	cpu_locked
	.type	soft_reset, %function
