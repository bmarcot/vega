# CPU must be one of the following: cortex-m0 cortex-m3 cortex-m4
CPU= cortex-m3
MACHINE= lm3s6965evb

ifeq ($(CPU),cortex-m3)
	ARCH = armv7-m
else ifeq ($(CPU),cortex-m4)
	ARCH = armv7e-m
else ifeq ($(CPU),cortex-m0)
	ARCH = armv6-m
endif

# vegaz, compressed kernel
NAME = vega

CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AS = $(CROSS)as
OCPY = $(CROSS)objcopy
HOSTCC=gcc
CFLAGS = -mcpu=$(CPU) -march=$(ARCH) -mthumb -Iinclude
LDFLAGS = -Wl,-T$(MACHINE).ld

SSRC += head.S entry.S syscalls.S kernel-if.S
CSRC += main.c uart.c systick.c backend.c thread.c sched-rr.c sysvect.c \
	sys/pthread.c fault.c bitmap.c mm.c mutex.c
OBJS += $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: lm3s6965evb.ld $(NAME).hex

$(NAME).elf: $(OBJS)
	$(CC) -mthumb -march=$(ARCH) -nostartfiles -Wl,-Map=$(NAME).map $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -nostartfiles -std=gnu99 $<

%.o: %.S
	$(CC) -o $@ $(CFLAGS) -c $<

%.ld: %.ld.S
	$(HOSTCC) -E -P -Iinclude -o $@ $<

%.hex: %.elf
	$(OCPY) -O ihex $< $@

clean::
	rm -f *.o *~ $(NAME).map *.ld

distclean: clean
	rm -f $(NAME).elf $(NAME).hex

run: $(NAME).elf
	qemu-system-arm -serial stdio -nographic -cpu $(CPU) -machine $(MACHINE) -kernel $^
