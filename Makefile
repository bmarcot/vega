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
CSRC = main.c uart.c systick.c backend.c page.c test/list.c thread.c sched-rr.c sysvect.c sys/pthread.c
OBJS = $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: entry.elf

entry.elf: $(OBJS)
	$(CC) -mthumb -march=armv7e-m -nostartfiles -Wl,-Map=entry.map $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -nostartfiles -std=gnu99 $<

#%.o: %.S
#	$(AS) -o $@ $(CFLAGS) $<
%.o: %.S
	$(HOSTCC) -E -Iinclude $< | $(CC) -o $@ $(CFLAGS) -c -xassembler -

clean:
	rm -f *.o test/*.o *~ entry.map

distclean: clean
	rm -f entry.elf

run: entry.elf
	qemu-system-arm -serial stdio -nographic -cpu $(CPU) -machine $(MACHINE) -kernel $^
