CPU= cortex-m3
MACHINE= lm3s6965evb

CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AS = $(CROSS)as
HOSTCC=gcc
# LD = $(CROSS)gcc
CFLAGS = -mcpu=$(CPU) -mthumb -Iinclude
LDFLAGS = -Wl,-T$(MACHINE).ld

SSRC = head.S entry.S syscalls.S
CSRC = main.c uart.c systick.c backend.c page.c test/list.c thread.c sched-rr.c sysvect.c \
	sys/pthread.c user/main.c fault.c
OBJS = $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: lm3s6965evb.ld kernel.elf

kernel.elf: $(OBJS)
	$(CC) -mthumb -march=armv7e-m -nostartfiles -Wl,-Map=kernel.map $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -nostartfiles -std=gnu99 $<

#%.o: %.S
#	$(AS) -o $@ $(CFLAGS) $<
%.o: %.S
	$(HOSTCC) -E -Iinclude $< | $(CC) -o $@ $(CFLAGS) -c -xassembler -

%.ld: %.ld.S
	 $(HOSTCC) -E -P -Iinclude -o $@ $<

clean:
	rm -f *.o test/*.o *~ kernel.map *.ld

distclean: clean
	rm -f kernel.elf

run: kernel.elf
	qemu-system-arm -serial stdio -nographic -cpu $(CPU) -machine $(MACHINE) -kernel $^
