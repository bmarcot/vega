/*
 * platform/nrf52/gpio.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include "platform.h"

#define GPIO_DIR_OUT  0
#define GPIO_DIR_IN   1

static void set_gpio_direction(unsigned int offset, int dir)
{
	if (dir == GPIO_DIR_OUT) {
		NRF_P0->PIN_CNF[offset] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos)
			| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
			| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
			| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
			| (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
	} else {
		NRF_P0->PIN_CNF[offset] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos)
			| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
			| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
			| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
			| (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
	}
}

void nrf52_gpio_set(unsigned int offset, int value)
{
	if (value)
		NRF_P0->OUTSET = 1 << offset;
	else
		NRF_P0->OUTCLR = 1 << offset;
}

int nrf52_gpio_get(unsigned int offset)
{
	return (NRF_P0->IN >> offset) & 1;
}

int nrf52_gpio_direction_input(unsigned int offset)
{
	set_gpio_direction(offset, GPIO_DIR_IN);

	return 0;
}

int nrf52_gpio_direction_output(unsigned int offset, int value)
{
	set_gpio_direction(offset, GPIO_DIR_OUT);
	nrf52_gpio_set(offset, value);

	return 0;
}
