/*
 * platform/nrf52/platform.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _PLATFORM_NRF52_PLATFORM_H
#define _PLATFORM_NRF52_PLATFORM_H

#include <cmsis/nrf52/nrf52.h>
#include <cmsis/nrf52/nrf52_bitfields.h>
#include <cmsis/nrf52/nrf52832_peripherals.h>

struct spim_config {
	unsigned int pin_sck;
	unsigned int pin_mosi;
	unsigned int pin_miso;
	unsigned int pin_cs;
	unsigned int freq;
};

struct twim_config {
	unsigned int pin_scl;
	unsigned int pin_sda;
	unsigned int freq;
	unsigned int address;
};

void __platform_init(void);
void __platform_halt(void);

#endif /* !_PLATFORM_NRF52_PLATFORM_H */
