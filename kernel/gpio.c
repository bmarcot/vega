#include <sys/types.h>

#include "asm-io.h"

/*
NRF_P0->PIN_CNF[17] = (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);

for (;;) {
	NRF_P0->OUTCLR = 1 << 17;
	msleep(30);
	NRF_P0->OUTSET = 1 << 17;
	msleep(470);
*/

//#include <drivers/gpio.h>

#define GPIO_DIR_OUT  0
#define GPIO_DIR_IN   1

static void set_gpio_direction(struct gpio_chip *chip, unsigned int offset, int dir)
{
	(void) chip;

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

static void nrf52_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	(void) chip;

	if (value)
		NRF_P0->OUTSET = 1 << offset;
	else
		NRF_P0->OUTCLR = 1 << offset;
}

static int nrf52_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	return (NRF_P0->IN >> offset) & 1;
}

static int nrf52_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	set_gpio_direction(chip, offset, GPIO_DIR_IN);

	return 0;
}

static int nrf52_gpio_direction_output(struct gpio_chip *chip, unsigned offset,
				int value)
{
	set_gpio_direction(chip, offset, GPIO_DIR_OUT);
	nrf52_gpio_set(chip, offset, value);

	return 0;
}

static struct gpio_chip nrf52_gpio_chip = {
	.label = "nRF52",
	.direction_input = nrf52_gpio_direction_input,
	.get = nrf52_gpio_get,
	.direction_output = nrf52_gpio_direction_output,
	.set = nrf52_gpio_set,
	/* .base= 0, */
	.ngpio = 32,
};

void __init nrf52_gpio_init(void)
{
	int i, j;

	for (i = 0; i < GPIO_PORT_NUM; i++) {
		/* disable, unmask and clear all interrupts */
		__raw_writel(0x0, GPIO_BASE(i) + GPIO_INT_EN);
		__raw_writel(0x0, GPIO_BASE(i) + GPIO_INT_MASK);
		__raw_writel(~0x0, GPIO_BASE(i) + GPIO_INT_CLR);

		for (j = GPIO_IRQ_BASE + i * 32;
		     j < GPIO_IRQ_BASE + (i + 1) * 32; j++) {
			irq_set_chip_and_handler(j, &gpio_irq_chip,
						handle_edge_irq);
			irq_clear_status_flags(j, IRQ_NOREQUEST);
		}

		irq_set_chained_handler_and_data(IRQ_GPIO(i), gpio_irq_handler,
						(void *)i);
	}

	BUG_ON(gpiochip_add_data(&gemini_gpio_chip, NULL));
}

#ifdef CONFIG_HAS_GPIO
#endif

void gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	gpio_chip->gpio_set(chip, offset, value);
}

int gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	return gpio_chip->gpio_get(chip, offset);
}

int gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	return gpio_chip->gpio_direction_input(chip, offset);
}

int gpio_direction_output(struct gpio_chip *chip, unsigned offset,
			int value)
{
	return gpio_chip->gpio_direction_output(chip, offset, value);
}
