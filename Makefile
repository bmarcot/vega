
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
CFLAGS += -mcpu=$(CPU) -mthumb -Iinclude -Iinclude/libc -Iplatform/$(PLATFORM) -Wno-main

# ld must know the architecture because we use the stdlib (printf, memcpy..)
LDFLAGS = -mthumb -march=$(ARCH) -nostartfiles -Wl,-Map=$(NAME).map -Wl,-Tvega.lds

#FIXME: revisit the arch-specific imports
ifeq ($(ARCH),armv6-m)
	SSRC += v6m-entry.S
	CSRC += v6m-faults.c
else
	SSRC += v7m-entry.S v7m-if.S
	CSRC += v7m-faults.c
endif

SSRC += head.S common.S kernel-if.S
CSRC += main.c sys/pthread.c bitmap.c  sleep.c \
	unistd.c utils.c sys/resource.c
CSRC += $(wildcard libc/*.c)
CSRC += $(wildcard kernel/*.c)
OBJS += $(SSRC:.S=.o)
OBJS += $(CSRC:.c=.o)

all: include/cmsis/arm include/version.h $(NAME).lds $(NAME).hex

$(NAME).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -W -Wall -std=c11 -D__KERNEL__ $<

%.o: %.S
	$(CC) -o $@ $(CFLAGS) -c $<

%.lds: %.lds.S
	$(HOSTCC) -E -P -Iinclude -D__LINKER__ -DROMSZ=$(ROMSZ) -DRAMSZ=$(RAMSZ) -o $@ $<

include/version.h: include/version.template.h
	cat $< | sed -e "s/GIT_COMMIT/`git log --pretty=format:'%h' -n 1`/g" \
	-e "s/GIT_BRANCH/`git symbolic-ref --short HEAD`/g" > $@

include/cmsis/arm:
	svn export --force https://github.com/ARM-software/CMSIS/trunk/CMSIS/Include include/cmsis/arm
	svn export --force https://github.com/ARM-software/CMSIS/trunk/Device/ARM/ARMCM4/Include include/cmsis/arm
	svn export --force https://github.com/ARM-software/CMSIS/trunk/Device/ARM/ARMCM0/Include include/cmsis/arm

%.hex: %.elf
	$(OCPY) -O ihex $< $@

clean::
	rm -f *.o libc/*.o kernel/*.o sys/*.o *~ $(NAME).map $(NAME).lds include/version.h

distclean: clean
	rm -f $(NAME).elf $(NAME).hex

# platform Makefile.rules contains flashing and running rules
include platform/$(PLATFORM)/Makefile.rules
