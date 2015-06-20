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
	.long	0x20004000			@ FIXME: stack base on lm3s6965evb
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
	.long	0				@ Unused
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
	.long	0				@ SysTick
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

	.set	V7M_SCB_BASE, 0xe000ed00
	.set	V7M_SHCSR, 0x24
	.set    V7M_SHCSR_USGFAULTENA, (1 << 18)
	.set    V7M_SHCSR_BUSFAULTENA, (1 << 17)
	.set    V7M_SHCSR_MEMFAULTENA, (1 << 16)

	.balign	8
	.global	cold_reset
cold_reset:
	ldr     r0, =V7M_SCB_BASE
	ldr   	r1, =(V7M_SHCSR_USGFAULTENA | V7M_SHCSR_BUSFAULTENA | V7M_SHCSR_MEMFAULTENA)
	str   	r1, [r0, #V7M_SHCSR]
	dsb	sy
	isb	sy
	b	start_kernel

	.balign	8
warm_reset:
	b	cold_reset

	early_fault early_hardfault, "early exception: HardFault\n"
	early_fault early_memmanage, "early exception: MemManage\n"
	early_fault early_busfault, "early exception: BusFault\n"
	early_fault early_usagefault, "early exception: UsageFault\n"

	.balign 16
