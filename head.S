
	.syntax unified
	.thumb

#include "arch-v7m.h"
#include "linux/linkage.h"

	.include "macros.S"

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
	.long	early_stack_base		@ SP_main value
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
	.long	early_stack_base		@ SP_main value
	.long	(warm_reset + THUMB_MODE)	@ Reset (warm)
	.long	0				@ NMI
	.long	(fault + THUMB_MODE)	@ HardFault
	//FIXME: Qemu does not escalate other faults to #hf
	.long	(fault + THUMB_MODE)	@ HardFault
	.long	(fault + THUMB_MODE)	@ HardFault
	.long	(fault + THUMB_MODE)	@ HardFault
	//.long 	0				@ MemManage
	//.long 	0				@ BusFault
	//.long 	0				@ UsageFault
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	(svcall + THUMB_MODE)		@ SVCall
	.long	0				@ Debug Monitor
	.long	0				@ Reserved
	.long	0				@ PendSV
	.long	(systick + THUMB_MODE)		@ SysTick
	.long	0				@ External Interrupt
	// FIXME: the other handlers for exceptions and external
	// interrupts go here...

	.text

ENTRY(cold_reset)
	movl	r0, V7M_SCB_BASE

	@ switch to Handler_Mode
	ldr	r1, =vtor_trampoline_base
	str	r1, [r0, #V7M_VTOR]
	ldr	r2, =0f
	adds	r2, #1
	str	r2, [r1, #V7M_VTOR_OFFSET_SVC]
	sync
	svc	#0
0:

	@ restore the early vector
	ldr	r1, =vtor_early_base
	str	r1, [r0, #V7M_VTOR]

	@ enable usgf, busf, memf
	ldr   	r1, =(V7M_SHCSR_USGFAULTENA | V7M_SHCSR_BUSFAULTENA | V7M_SHCSR_MEMFAULTENA)
	str   	r1, [r0, #V7M_SHCSR]
	sync

	@ Copy the .data section from Flash to RAM.
	ldr	r2, =data_sect_size
	tst	r2, r2
	beq	1f
	ldr	r1, =data_sect_flash
	ldr	r0, =data_sect_ram
0:      ldr     r3, [r1], #4 // must be 32-byte aligned
	str     r3, [r0], #4
	ldr     r3, [r1], #4
	str     r3, [r0], #4
	ldr     r3, [r1], #4
	str     r3, [r0], #4
	ldr     r3, [r1], #4
	str     r3, [r0], #4
	ldr     r3, [r1], #4
	str     r3, [r0], #4
	ldr     r3, [r1], #4
	str     r3, [r0], #4
	ldr     r3, [r1], #4
	str     r3, [r0], #4
	ldr     r3, [r1], #4
	str     r3, [r0], #4
	subs    r2, #32
	bgt     0b
1:

	@ The start_kernel function returns NULL or a pointer to the thread_info struct
	@ of the first thread to run on the CPU. If the pointer is NULL the CPU is
	@ software reseted (could be reseted differently). If the pointer is not NULL
	@ the first thread context is kickstarted. 
	bl	start_kernel	@ do the high-level kernel init
	cmp	r0, #0		@ the proc returns the first thread to run
	it	eq		@ compare to NULL
	beq	soft_reset	@ reset if no threads to run
	b	thread_restore
ENDPROC(cold_reset)

ENTRY(warm_reset)
	b	cold_reset
ENDPROC(warm_reset)

ENTRY(fault)
	push	{r4-r11}	@ save non-scratch registers at time of fault
	mov	r0, sp
	mov	r2, lr		@ lr contains EXC_RETURN, fault was taken in thread or interrupt?

	@ We don't handle faults taken in interrupt handler at the moment.
	@ Hence the scratch registers auto pushed to the stack by the CPU
	@ on interrupt-entry have been pushed to PSP (MSP is only used by
	@ interrupts handler).
	mrs	r1, psp

	b	hardfault
ENDPROC(fault)

	.macro	early_fault f, m
	.balign	8
	.global	\f
\f:	adr	r0, 1f
	bl	uart_putstring
0:	b	0b
1:	.string	"\m"
	.endm	/* early_fault */

	early_fault early_hardfault, "early exception: HardFault\n"
	early_fault early_memmanage, "early exception: MemManage\n"
	early_fault early_busfault, "early exception: BusFault\n"
	early_fault early_usagefault, "early exception: UsageFault\n"

	.balign 16
