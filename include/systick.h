#ifndef SYSTICK_H
#define SYSTICK_H

#include "linux/types.h"

#define SYSTICK_FREQ_IN_HZ 1000
#define SYSTICK_PERIOD_IN_MSECS (SYSTICK_FREQ_IN_HZ / 1000)

struct systick {
	volatile u32 syst_csr;
	u32 syst_rvr;
	volatile u32 syst_cvr;
	u32 syst_calib;
};

#define SYST_CSR_CLKSOURCE (1 << 2)
#define SYST_CSR_TICKINT (1 << 1)
#define SYST_CSR_ENABLE 1
#define SYST_RELOAD_VAL ((1 << 24) - 1)
#define SYST_CSR_CLKSOURCE_CPU (1 << 2)

void systick_init(u32);
unsigned long get_clocktime_in_msec(void);
void systick(void);

static struct systick *const syst = (void *) 0xe000e010;

static inline void systick_enable(void)
{
	syst->syst_csr |= SYST_CSR_ENABLE;
	//FIXME: need synchronization?
}

static inline void systick_disable(void)
{
	syst->syst_csr &= ~SYST_CSR_ENABLE;
	//FIXME: need synchronization?
}

#endif /* !SYSTICK_H */
