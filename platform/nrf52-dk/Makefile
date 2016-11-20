CPU= cortex-m4
ARCH= armv7e-m

CFLAGS += -DNRF52 -Iinclude/cmsis/arm -Iinclude/cmsis/nrf52

ROMSZ = 256k
RAMSZ = 64k

CSRC += platform/nrf52-dk/uart.c	\
	platform/nrf52-dk/gpio.c	\
	platform/nrf52-dk/time.c	\
	platform/nrf52-dk/timer.c	\

SSRC += platform/nrf52-dk/entry.S	\
