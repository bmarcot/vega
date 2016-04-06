
# vegaz, compressed kernel
NAME = vega

# platform Makefile contains hw details and flags
include platform/$(PLATFORM)/Makefile

CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AS = $(CROSS)as
OCPY = $(CROSS)objcopy
HOSTCC=gcc

CFLAGS += -mcpu=$(CPU) -mthumb -Iinclude
ifeq ($(FREESTANDING),1)
CFLAGS += -DFREESTANDING -Wno-main
endif

# ld must know the architecture because we use the stdlib (printf, memcpy..)
LDFLAGS = -mthumb -march=$(ARCH) -nostartfiles -Wl,-Map=$(NAME).map -Wl,-Tvega.lds

SSRC += head.S entry.S syscalls.S kernel-if.S
CSRC += main.c uart.c systick.c backend.c thread.c sched-rr.c sysvect.c \
	sys/pthread.c faults.c bitmap.c mm.c mutex.c printk.c
OBJS += $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: $(NAME).lds $(NAME).hex

$(NAME).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -nostartfiles -std=gnu99 $<

%.o: %.S
	$(CC) -o $@ $(CFLAGS) -c $<

%.lds: %.lds.S
	$(HOSTCC) -E -P -Iinclude -D__LINKER__ -DROMSZ=$(ROMSZ) -DRAMSZ=$(RAMSZ) -o $@ $<

%.hex: %.elf
	$(OCPY) -O ihex $< $@

clean::
	rm -f *.o *~ $(NAME).map $(NAME).lds

distclean: clean
	rm -f $(NAME).elf $(NAME).hex

# platform Makefile.rules contains flashing and running rules
include platform/$(PLATFORM)/Makefile.rules
