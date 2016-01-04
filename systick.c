#include <stdint.h>

#include "systick.h"
#include "uart.h"
#include "sched-rr.h"

static volatile uint32_t overflow = 0;

void systick(void)
{
	uart_putstring("SysTick interrupt...\n");
	if (overflow == 0xff)
		overflow = 0;
	overflow++;

	//FIXME: not a good place to call the scheduler...
	sched_rr_elect();
}

void systick_init(uint32_t rvr)
{
	syst->syst_rvr = rvr;
	syst->syst_cvr = 0;
	syst->syst_csr |= (SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT);
	//FIXME: need synchronization?
}

uint32_t gettick(void)
{
	uint32_t of = overflow;
	uint32_t val = syst->syst_cvr;
	uint32_t new_of = overflow;

	if (of != new_of) {
		val = syst->syst_cvr;
		of = new_of;
		uart_putstring("SysTick overflowed.\n");
	}

	return (of << 24) | (SYST_RELOAD_VAL - val);
}

//TODO: calib_pseudo_10ms_wait()
