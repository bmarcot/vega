
# vegaz, compressed kernel
NAME = vega

# platform Makefile contains hw details and flags
include platform/$(PLATFORM)/Makefile

CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AS = $(CROSS)as
OCPY = $(CROSS)objcopy
HOSTCC=gcc

# warning: return type of 'main' is not 'int' [-Wmain]
CFLAGS += -mcpu=$(CPU) -mthumb -Iinclude -Iplatform/$(PLATFORM) -Wno-main

# ld must know the architecture because we use the stdlib (printf, memcpy..)
LDFLAGS = -mthumb -march=$(ARCH) -nostartfiles -Wl,-Map=$(NAME).map -Wl,-Tvega.lds

SSRC += head.S entry.S syscalls.S kernel-if.S
CSRC += main.c systick.c backend.c thread.c sched-rr.c sysvect.c \
	sys/pthread.c faults.c bitmap.c mm.c mutex.c printk.c sleep.c \
	timer.c
OBJS += $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: include/cmsis/arm include/version.h $(NAME).lds $(NAME).hex

$(NAME).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -std=gnu99 $<

%.o: %.S
	$(CC) -o $@ $(CFLAGS) -c $<

%.lds: %.lds.S
	$(HOSTCC) -E -P -Iinclude -D__LINKER__ -DROMSZ=$(ROMSZ) -DRAMSZ=$(RAMSZ) -o $@ $<

include/version.h: include/version_template.h
	cat $< | sed -e "s/GIT_COMMIT/`git log --pretty=format:'%h' -n 1`/g" \
	-e "s/GIT_BRANCH/`git symbolic-ref --short HEAD`/g" > $@

include/cmsis/arm:
	svn export --force https://github.com/ARM-software/CMSIS/trunk/CMSIS/Include include/cmsis/arm
	svn export --force https://github.com/ARM-software/CMSIS/trunk/Device/ARM/ARMCM4/Include include/cmsis/arm

%.hex: %.elf
	$(OCPY) -O ihex $< $@

clean::
	rm -f *.o *~ $(NAME).map $(NAME).lds include/version.h

distclean: clean
	rm -f $(NAME).elf $(NAME).hex

# platform Makefile.rules contains flashing and running rules
include platform/$(PLATFORM)/Makefile.rules
