/*  PRESCALER      FREQ.   PERIOD    CLOCK
 *  ---------  ---------  -------  -------
 *          0     16 MHz  62.5 ns  PCLK16M
 *          1      8 MHz   125 ns        -
 *          2      4 MHz   250 ns        -
 *          3      2 MHz   500 ns        -
 *          4      1 MHz     1 us   PCLK1M  (default)
 *          5    500 kHz     2 us        -
 *          6    250 kHz     4 us        -
 *          7    125 kHz     8 us        -
 *          8   62.5 kHz    16 us        -
 *          9  31.25 kHz    32 us        -
 */

#include <kernel/irq.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/types.h>

#include "cmsis/nrf52/nrf.h"
#include "linux/list.h"


#include <signal.h>
#include <string.h>
#include <kernel/errno-base.h>
#include <kernel/time.h>

static const u8 prescalers[] = { 9, 4, 5, 4, 6, 4, 5, 4,
				 7, 4, 5, 4, 6, 4, 5, 4,
				 8, 4, 5, 4, 6, 4, 5, 4,
				 7, 4, 5, 4, 6, 4, 5, 4 };

static const u8 usec_per_tick[] = { 1, 2, 4, 8, 16, 32 };

static const u8 bitmodes[] = { TIMER_BITMODE_BITMODE_32Bit,
			       TIMER_BITMODE_BITMODE_24Bit,
			       TIMER_BITMODE_BITMODE_16Bit,
			       TIMER_BITMODE_BITMODE_08Bit };

struct timer_list {
	NRF_TIMER_Type *nrf_timer;
	struct thread_info *owner;
	struct list_head list;
	enum timer_type timer_type;
	void *timer_data;
};

static struct timer_list timer_list[5];    /* TIMER0 to TIMER4 */

static struct sigevent sigevents[5];

static LIST_HEAD(timers); // free_timers

#define ACTIVE_THREAD ({				  \
	struct thread_info *__tp = current_thread_info(); \
	__tp; })

static void nrf52_timer_irq(int irq)
{
	struct thread_info *owner;

	/* acknowledge interrupt in the peripheral */
	timer_list[irq].nrf_timer->EVENTS_COMPARE[0] = 0;

	if (timer_list[irq].timer_type == TT_SLEEP) {
		/* get the owner thread, add to runqueue */
		owner = timer_list[irq].owner;
		sched_enqueue(owner);
		if (owner->ti_priority >= ACTIVE_THREAD->ti_priority)
			sched_elect(SCHED_OPT_NONE);
	} else {
		printk("OK! OK! OK! timer exprired!\n");
		/* FIXME: Sigevent handler is staged in current thread
		   at the moment. It should be staged in timer's owner
		   thread instead. */
		do_sigevent(&sigevents[irq]);

		/* return the timer to the list of free timers */
		list_add(&timer_list[irq].list, &timers);
	}
}

static void nrf52_timer0_irq(void) { nrf52_timer_irq(0); }
static void nrf52_timer1_irq(void) { nrf52_timer_irq(1); }
static void nrf52_timer2_irq(void) { nrf52_timer_irq(2); }
static void nrf52_timer3_irq(void) { nrf52_timer_irq(3); }
static void nrf52_timer4_irq(void) { nrf52_timer_irq(4); }

void nrf52_timer_init(void)
{
	struct timer_list *pos;

	/* build the list of timers available in the system */
	timer_list[0].nrf_timer = NRF_TIMER0;
	timer_list[1].nrf_timer = NRF_TIMER1;
	timer_list[2].nrf_timer = NRF_TIMER2;
	timer_list[3].nrf_timer = NRF_TIMER3;
	timer_list[4].nrf_timer = NRF_TIMER4;
	for (int i = 4; i >= 0; i--)
		list_add(&timer_list[i].list, &timers);

	/* configure each timer */
	list_for_each_entry(pos, &timers, list) {
		pos->nrf_timer->MODE = TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
		pos->nrf_timer->INTENSET =
			TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos;
		pos->nrf_timer->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos)
			| (TIMER_SHORTS_COMPARE0_STOP_Enabled << TIMER_SHORTS_COMPARE0_STOP_Pos);
	}

	irq_attach(TIMER0_IRQn, nrf52_timer0_irq);
	irq_attach(TIMER1_IRQn, nrf52_timer1_irq);
	irq_attach(TIMER2_IRQn, nrf52_timer2_irq);
	irq_attach(TIMER3_IRQn, nrf52_timer3_irq);
	irq_attach(TIMER4_IRQn, nrf52_timer4_irq);

	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER2_IRQn);
	NVIC_EnableIRQ(TIMER3_IRQn);
	NVIC_EnableIRQ(TIMER4_IRQn);
}

int get_bitmode(unsigned long cc)
{
	printk("  cc=%x\n", cc);
	if (cc & 0xff000000)
		return TIMER_BITMODE_BITMODE_32Bit;
	else if (cc & 0xff0000)
		return TIMER_BITMODE_BITMODE_24Bit;
	else if (cc & 0xff00)
		return TIMER_BITMODE_BITMODE_16Bit;
	return TIMER_BITMODE_BITMODE_08Bit;
}

static void configure_timer(unsigned long usec, struct timer_list *timerp)
{
	int prescaler, bitmode, cc;

	/* compute the minimum number of comparisons */
	prescaler = prescalers[usec % 32];
	cc = usec / usec_per_tick[prescaler - 4];
	//bitmode = bitmodes[__builtin_clz(cc) / 4];
	bitmode = get_bitmode(cc);

	printk("nrf52: allocated timer X - ps=%d, cc=%d, bm=%d\n", prescaler, cc, bitmode);

	/* setup the peripheral */
	timerp->nrf_timer->CC[0] = cc;
	timerp->nrf_timer->BITMODE = bitmode << TIMER_BITMODE_BITMODE_Pos;
	timerp->nrf_timer->PRESCALER = prescaler << TIMER_PRESCALER_PRESCALER_Pos;
	timerp->nrf_timer->TASKS_START = 1;
}

// deprecated in POSIX, should use nanosleep instead
int __usleep(unsigned int usec)
{
	struct timer_list *timer;

	/* if (usec >= 10e6) */
	/* 	return -1; //FXIME: and errno = EINVAL; */

	/* pick a timer in the list of free timers */
	timer = list_first_entry_or_null(&timers, struct timer_list, list);
	if (timer == NULL)
		return -1;  //FIXME: and ERrNO = ENOTIMER;
	list_del(&timer->list);

	timer->timer_type = TT_SLEEP;
	configure_timer(usec, timer);

	/* update thread's control block */
	timer->owner = current_thread_info();
	timer->owner->ti_state = THREAD_STATE_BLOCKED;

	sched_elect(SCHED_OPT_NONE);

	/* return the timer to the list of free timers */
	list_add(&timer->list, &timers);

	return 0;
}

#define USEC_PER_MSEC  1000

int __msleep(unsigned int msec)
{
	return __usleep(msec * USEC_PER_MSEC);
}

/* void nrf52_timer_cancel() */
/* { */
/* 	timer->nrf_timer->TASKS_STOP = 1; */
/* 	timer->nrf_timer->EVENTS_COMPARE[0] = 0; */

/* 	// which TIMERx is getting cancelled?? */
/* 	NVIC_ClearPendingIRQ(TIMER0_IRQn); */
/* } */

/* int sys_timer_create(unsigned int msec) */
/* { */
/* 	(void)msec; */

/* 	return -1; */
/* } */

void __systick(void)
{
}

#define ARRAY_INDEX(x, xs) \
	(((unsigned long)x - (unsigned long)xs) / sizeof(__typeof__(*x)))

int sys_timer_create(clockid_t clockid, struct sigevent *sevp,
		timer_t *timerid)
{
	struct timer_list *timer;
	CURRENT_THREAD_INFO(tip);

	(void)clockid;

	printk("Sys Timer Create - NRF52\n");

	/* pick a timer in the list of free timers */
	timer = list_first_entry_or_null(&timers, struct timer_list, list);
	if (timer == NULL)
		return -1;  //FIXME: and ERrNO = ENOTIMER;
	list_del(&timer->list);

	//*timerid = (int)((char *)timer - (char *)timer_list) / sizeof(struct timer_list);
	*timerid = ARRAY_INDEX(timer, timer_list);
	timer->owner = tip;
	timer->timer_type = TT_TIMER;
	timer->timer_data = &sigevents[*timerid];
	memcpy(&sigevents[*timerid], sevp, sizeof(struct sigevent));
	//list_add(&timerp->list, &inactive_timers); // add to list of non-armed timer

	return 0;
}

/* int timer_settime(timer_t timerid, int flags, */
/* 		const struct itimerspec *new_value, */
/* 		struct itimerspec * old_value) */
int timer_settime(timer_t timerid, int flags, int new_value)
{
	(void)flags;

	struct timer_list *timerp = &timer_list[timerid];

	if (timerp == NULL) {
		printk("timer_settime: No timer found with id=%d\n", timerid);
		return -EINVAL;
	}
	/* timerp->expire_clocktime = get_clocktime_in_msec() + new_value; */
	/* list_move(&timerp->list, &timers); */
	configure_timer(new_value, timerp);

	return 0;

}
