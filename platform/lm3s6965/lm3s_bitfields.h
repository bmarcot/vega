#ifndef _LM3S_BITFIELDS_H
#define _LM3S_BITFIELDS_H

/*
 * General-Purpose Timers
 */

/* GPTM Configuration (GPTMCFG) */
#define GPTM_GPTMCFG_GPTMCFG_Pos	0
#define GPTM_GPTMCFG_GPTMCFG_32BitTimer	0

/* GPTM Control (GPTMCTL) */
#define GPTM_GPTMCTL_TAEN_Pos		0
#define GPTM_GPTMCTL_TAEN_Msk		(0x1 << GPTM_GPTMCTL_TAEN_Pos)
#define GPTM_GPTMCTL_TAEN_Enabled	0x1

/* GPTM TimerA Mode (GPTMTAMR) */
#define GPTM_GPTMTAMR_TAMR_Pos		0
#define GPTM_GPTMTAMR_TAMR_OneShot	0x1
#define GPTM_GPTMTAMR_TAMR_Periodic	0x2
#define GPTM_GPTMTAMR_TAMR_Capture	0x3

/* GPTM Interrupt Mask (GPTMIMR) */
#define GPTM_GPTMIMR_TATOIM_Pos		0
#define GPTM_GPTMIMR_TATOIM_Enabled	0x1

/* GPTM Interrupt Clear (GPTMICR) */
#define GPTM_GPTMICR_TATOCINT_Pos	0
#define GPTM_GPTMICR_TATOCINT_Cleared	0x1

#endif /* !_LM3S_BITFIELDS_H */
