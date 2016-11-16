#include "linux/linkage.h"

	.syntax unified
	.thumb

#include "pseudo.S"

	.section ".vector", "a"
	@ the vector must be aligned on 128 bytes (see VTOR reserved
	@ bits 6:0), alignment shall be handled in the linker script
	@ note: the SP has been set by the early vector at this point
	.long	__early_stack_start__		@ SP_main value
	.long	reset				@ Reset
	.long	0				@ NMI
	.long	hardf				@ HardFault
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	//FIXME: Qemu does not escalate other faults to #hf
	.long	memf				@ MemManage
	.long	busf				@ BusFault
	.long	usgf				@ UsageFault
#endif
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	svcall				@ SVCall
	.long	0				@ Debug Monitor
	.long	0				@ Reserved
	.long	0				@ PendSV
	.long	systick				@ SysTick
	.long	irq_entry			@ IRQ 0
	.long	irq_entry			@ IRQ 1
	.long	irq_entry			@ IRQ 2
	.long	irq_entry			@ IRQ 3
	.long	irq_entry			@ IRQ 4
	.long	irq_entry			@ IRQ 5
	.long	irq_entry			@ IRQ 6
	.long	irq_entry			@ IRQ 7
	.long	irq_entry			@ IRQ 8
	.long	irq_entry			@ IRQ 9
	.long	irq_entry			@ IRQ 10
	.long	irq_entry			@ IRQ 11
	.long	irq_entry			@ IRQ 12
	.long	irq_entry			@ IRQ 13
	.long	irq_entry			@ IRQ 14
	.long	irq_entry			@ IRQ 15
	.long	irq_entry			@ IRQ 16
	.long	irq_entry			@ IRQ 17
	.long	irq_entry			@ IRQ 18
	.long	irq_entry			@ IRQ 19
	.long	irq_entry			@ IRQ 20
	.long	irq_entry			@ IRQ 21
	.long	irq_entry			@ IRQ 22
	.long	irq_entry			@ IRQ 23
	.long	irq_entry			@ IRQ 24
	.long	irq_entry			@ IRQ 25
	.long	irq_entry			@ IRQ 26
	.long	irq_entry			@ IRQ 27
	.long	irq_entry			@ IRQ 28
	.long	irq_entry			@ IRQ 29
	.long	irq_entry			@ IRQ 30
	.long	irq_entry			@ IRQ 31
	.long	irq_entry			@ IRQ 32
	.long	irq_entry			@ IRQ 33
	.long	irq_entry			@ IRQ 34
	.long	irq_entry			@ IRQ 35
	.long	irq_entry			@ IRQ 36
	.long	irq_entry			@ IRQ 37
	.long	irq_entry			@ IRQ 38
	.long	irq_entry			@ IRQ 39
	.long	irq_entry			@ IRQ 40
	.long	irq_entry			@ IRQ 41
	.long	irq_entry			@ IRQ 42
	.long	irq_entry			@ IRQ 43
	.long	irq_entry			@ IRQ 44
	.long	irq_entry			@ IRQ 45
	.long	irq_entry			@ IRQ 46
	.long	irq_entry			@ IRQ 47
	.long	irq_entry			@ IRQ 48
	.long	irq_entry			@ IRQ 49
	.long	irq_entry			@ IRQ 50
	.long	irq_entry			@ IRQ 51
	.long	irq_entry			@ IRQ 52
	.long	irq_entry			@ IRQ 53
	.long	irq_entry			@ IRQ 54
	.long	irq_entry			@ IRQ 55
	.long	irq_entry			@ IRQ 56
	.long	irq_entry			@ IRQ 57
	.long	irq_entry			@ IRQ 58
	.long	irq_entry			@ IRQ 59
	.long	irq_entry			@ IRQ 60
	.long	irq_entry			@ IRQ 61
	.long	irq_entry			@ IRQ 62
	.long	irq_entry			@ IRQ 63

	.text

ENTRY(reset)
	movl	r0, 0xe000ed00			/* SCB_BASE */

	@ switch to Handler_Mode
	ldr	r1, =__early_stack_end__	/* allocate a temporary vector */
	str	r1, [r0, #8]			/* update VTOR */
	ldr	r2, =0f
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	movs	r3, #1				/* set thumb bit */
	orrs	r2, r3
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	orr	r2, #1				/* set thumb bit */
#endif
	str	r2, [r1, #11 * 4]		/* offset to SVC entry */
	dsb					/* [1] */
	svc	#0
0:

	@ restore the early vector
	ldr	r1, =__vector_start__
	str	r1, [r0, #8]			/* restore VTOR */
	dsb					/* [1] */

	@ Copy the .data section from Flash to RAM.
	ldr	r0, =__data_start__
	ldr	r1, =__rodata_end__
	ldr	r2, =__data_size__
	bl	memcpy

	@ Zero-fill the .bss sections.
	ldr	r0, =__bss_start__
	movs	r1, #0
	ldr	r2, =__bss_size__
	bl	memset

	@ The start_kernel function returns NULL or a pointer to the thread_info struct
	@ of the first thread to run on the CPU. If the pointer is NULL the CPU is
	@ software reseted (could be reseted differently). If the pointer is not NULL
	@ the first thread context is kickstarted. 
	bl	start_kernel	@ do the high-level kernel init
	cmp	r0, #0		@ the proc returns the first thread to run
	beq	0f		@ reset if system bootstrap failed
	b	thread_restore
0:	ldr	r0, =system_reset
	bx	r0
ENDPROC(reset)

/* [1] http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHDGBJG.html */

ENTRY(irq_entry)
	push	{lr}
	ldr	r0, =irq_handler
	mrs	r1, ipsr
	sub	r1, #16
	ldr.w	r0, [r0, r1, lsl #2]
	blx	r0
	pop	{pc}
ENDPROC(irq_entry)

	.balign 16
