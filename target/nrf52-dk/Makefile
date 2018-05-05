CPU	= cortex-m4
ARCH	= armv7e-m

ROMSZ	= 256k
RAMSZ	= 64k

CFLAGS	+= -Iplatform/nrf52

CSRC	+= platform/nrf52/init.c

# CMSIS files
CSRC	+= cmsis/nrf52/system_nrf52.c

# Serial driver files
CSRC	+= drivers/serial/nrf52.c

# Gpio driver files
CSRC	+= platform/nrf52/gpio.c

# Platform drivers
CSRC	+= platform/nrf52/spim.c
CSRC	+= platform/nrf52/twim.c

# Timer driver files
CSRC	+= drivers/clockevents/timer-nrf52.c

HRTIMER_DEVICE=nrf52-timer0
