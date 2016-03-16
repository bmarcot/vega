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

CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AS = $(CROSS)as
HOSTCC=gcc
# LD = $(CROSS)gcc
CFLAGS = -mcpu=$(CPU) -march=$(ARCH) -mthumb -Iinclude
LDFLAGS = -Wl,-T$(MACHINE).ld

SSRC = head.S entry.S syscalls.S
CSRC = main.c uart.c systick.c backend.c test/list.c thread.c sched-rr.c sysvect.c \
	sys/pthread.c user/main.c fault.c sysdummies.c bitmap.c mm.c
OBJS = $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: lm3s6965evb.ld kernel.elf

kernel.elf: $(OBJS)
	$(CC) -mthumb -march=$(ARCH) -nostartfiles -Wl,-Map=kernel.map $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -nostartfiles -std=gnu99 $<

%.o: %.S
	$(CC) -o $@ $(CFLAGS) -c $<

%.ld: %.ld.S
	 $(HOSTCC) -E -P -Iinclude -o $@ $<

clean:
	rm -f *.o test/*.o *~ kernel.map *.ld

distclean: clean
	rm -f kernel.elf

run: kernel.elf
	qemu-system-arm -serial stdio -nographic -cpu $(CPU) -machine $(MACHINE) -kernel $^
