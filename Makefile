CPU= cortex-m3
MACHINE= lm3s6965evb

CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)ld
CFLAGS = -mcpu=$(CPU) -mthumb
LDFLAGS = -Tentry.ld

SSRC = head.S entry.S
CSRC = main.c uart.c systick.c backend.c
OBJS = $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: entry.elf

entry.elf: $(OBJS)
	$(LD) -nodefaultlibs -nostartfiles -Map entry.map -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -nodefaultlibs -nostartfiles $<

%.o: %.S
	$(AS) -o $@ $(CFLAGS) $<

clean:
	rm -f *.o *~ entry.map

distclean: clean
	rm -f entry.elf

run: entry.elf
	qemu-system-arm -serial stdio -nographic -cpu $(CPU) -machine $(MACHINE) -kernel $^
