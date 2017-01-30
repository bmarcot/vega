CPU	= cortex-m4
ARCH	= armv7e-m

ROMSZ	= 256k
RAMSZ	= 64k

CFLAGS	+= -Iplatform/nrf52

CSRC	+= platform/nrf52/init.c \
	   platform/nrf52/hwrng.c

# CMSIS files
CSRC	+= cmsis/nrf52/system_nrf52.c

# Timer driver files
CSRC	+= drivers/timer/nrf52.c
SSRC	+= drivers/timer/nrf52-entry.S

# Serial driver files
CSRC	+= drivers/serial/nrf52.c

# Gpio driver files
CSRC	+= platform/nrf52/gpio.c

# Master SPIM
CSRC	+= platform/nrf52/spim.c
