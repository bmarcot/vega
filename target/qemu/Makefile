CPU	= cortex-m3
ARCH	= armv7-m

ROMSZ	= 256k
RAMSZ	= 64k

CFLAGS	+= -Iplatform/lm3s6965

CSRC	+=				\
	target/qemu/halt.c		\
	platform/lm3s6965/init.c	\

# CMSIS files
CSRC	+= cmsis/lm3s6965/system_lm3s.c

# Timer driver files
CSRC	+= drivers/timer/systick.c
CSRC	+= drivers/clockevents/timer-lm3s.c
CSRC	+= drivers/clocksource/systick.c

# Serial driver files
CSRC	+= drivers/serial/lm3s6965.c
