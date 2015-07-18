	.syntax unified
	.thumb

	.set	THUMB_MODE,	1

	@ early vector is actually defined by the platform:
	@    - stack base
	@    - platform-specific reset handler?
	@ why?
	@  we don't have support for printf() in the early
	@  stage, but we want some informative messages
	@  although they are not formatted with a lot of info
	@ how?
	@  use 2 separate vectors: one during the bootstrap,
	@ and one during the runtime
	@ what?
	@  one could say we could merge the two vectors, and
	@  locate that single vector into RAM space. Yet most
	@  platforms come with huge ROM compared to RAM, making
	@  the ROM space cheap wrt. the RAM. Another solution
	@  would be to have a branch condition in the fault
	@  handlers, but I find it less elegant, also that's
	@  a problem if a fault handler is often called in the
	@  future (think of pagefault/regionfault).

	.section ".vector.early", "a"

	@ the early vector is naturally aligned on the bottom of the
	@ ROM, also the VTOR is pointing to the early vector base
	.long	0x20004000			@ FIXME: stack base on lm3s6965evb //FIXME: BUG..BUG.. set the stack to pgpool_limit, or OVERLAY
	.long	(cold_reset + THUMB_MODE)	@ Reset (cold)
	.long	0				@ Unused
	.long	(early_hardfault + THUMB_MODE)	@ HardFault
	.long 	(early_memmanage + THUMB_MODE)	@ MemManage
	.long 	(early_busfault + THUMB_MODE)	@ BusFault
	.long 	(early_usagefault + THUMB_MODE)	@ UsageFault

	.section ".vector", "a"

	@ the vector must be aligned on 128 bytes (see VTOR reserved
	@ bits 6:0), alignment shall be handled in the linker script
	@ note: the SP has been set by the early vector at this point
	//.long	0				@ Unused
	.long	0x20004000			@ FIXME: stack base on lm3s6965evb //FIXME: BUG..BUG.. set the stack to pgpool_limit, or OVERLAY
	.long	(warm_reset + THUMB_MODE)	@ Reset (warm)
	.long	0				@ NMI
	.long	0				@ HardFault
	.long 	0				@ MemManage
	.long 	0				@ BusFault
	.long 	0				@ UsageFault
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ SVCall
	.long	0				@ Debug Monitor
	.long	0				@ Reserved
	.long	0				@ PendSV
	.long	(systick + THUMB_MODE)		@ SysTick
	.long	0				@ External Interrupt
	// FIXME: the other handlers for exceptions and external
	// interrupts go here...

	.text

	.macro	early_fault f, m
	.balign	8
	.global	\f
\f:	adr	r0, 1f
	bl	uart_putstring
0:	b	0b
1:	.string	"\m"
	.endm	/* early_fault */

	@ context synchronization
	.macro	sync
	dsb	sy
	isb	sy
	.endm	/* sync */

	//FIXME: dry! please move me to an include file.
	@ movl	rd, imm32
	.macro	movl r, v
	movw	\r, #(\v & 0xffff)
	movt	\r, #(\v >> 16)
	.endm	/* movl */

	.set	V7M_SCB_BASE, 0xe000ed00
	.set	V7M_SHCSR, 0x24
	.set    V7M_SHCSR_USGFAULTENA, (1 << 18)
	.set    V7M_SHCSR_BUSFAULTENA, (1 << 17)
	.set    V7M_SHCSR_MEMFAULTENA, (1 << 16)
	.set	V7M_VTOR, 0x08
	.set	V7M_VTOR_OFFSET_SVC, 44

        .set    EXC_RETURN_HANDLER_MAIN, 0xfffffff1
	.set    EXC_RETURN_THREAD_MAIN, 0xfffffff9
	.set	EXC_RETURN_THREAD_PROCESS, 0xfffffffd

	.set    XPSR_T, 1 << 24

	.balign	8
	.global	cold_reset
cold_reset:
	movl	r0, V7M_SCB_BASE

	@ switch to Handler_Mode
	ldr	r1, =vtor_trampoline_base
	str	r1, [r0, #V7M_VTOR]
	ldr	r2, =1f
	adds	r2, #1
	str	r2, [r1, #V7M_VTOR_OFFSET_SVC]
	sync
	svc	#0
1:

	@ restore the early vector
	ldr	r1, =vtor_early_base
	str	r1, [r0, #V7M_VTOR]

	@ enable usgf, busf, memf
	ldr   	r1, =(V7M_SHCSR_USGFAULTENA | V7M_SHCSR_BUSFAULTENA | V7M_SHCSR_MEMFAULTENA)
	str   	r1, [r0, #V7M_SHCSR]
	sync

	bl	start_kernel	@ do the high-level kernel init
	cmp	r0, #0
	it	ne
	blne	soft_reset	@ reset if the init failed

	@ craft the thread interrupt stackframe
	mov     r0, #0		@  sp+0 r0
	mov     r1, #0		@  sp+4 r1
	mov     r2, #0		@  sp+8 r2
	mov     r3, #0		@ sp+12 r3
	mov     r4, #0		@ sp+16 r12
	mov     r5, #0		@ sp+20 lr (todo: call to libc/userlib exit())
	ldr     r6, =1f		@ sp+24 return_addr
	//add	r6, #1		@ FIXME: no need to enable Thumb bit?
	ldr     r7, =XPSR_T	@ sp+28 xPSR
	mov     r0, sp		@ force sp alignment to 8-byte
	and     r0, #~7
	mov	sp, r0
	stmdb   sp!, {r0-r7}	@ push the stack to memory
	mov	r0, sp
	msr     psp, r0		@ initialize the SP_process

	@ return from interrupt in Thread_Mode, SP_process (, Unprivileged (todo))
	ldr     lr, =EXC_RETURN_THREAD_PROCESS
	bx      lr
2:	b	2b		@ the cpu should never get there

	.balign	8
warm_reset:
	b	cold_reset

	early_fault early_hardfault, "early exception: HardFault\n"
	early_fault early_memmanage, "early exception: MemManage\n"
	early_fault early_busfault, "early exception: BusFault\n"
	early_fault early_usagefault, "early exception: UsageFault\n"

	.balign 16
