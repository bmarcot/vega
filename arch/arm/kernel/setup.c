/*
 * arch/arm/kernel/setup.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include "platform.h"

void setup_arch(void)
{
	/* enable UsageFault, BusFault, MemManage */
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

	/* ensure 8-byte stack alignment */
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	/* architectural requirement */
	__DSB();
}
