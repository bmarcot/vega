
# Control the build verbosity
ifeq ("$(VERBOSE)","1")
Q :=
VECHO = @true
else
Q := @
VECHO = @echo
endif

# vegaz, compressed kernel
NAME = vega

# Build and run on Qemu when the target is unspecified
TARGET ?= qemu

DIRS = . kernel libc api include include/libc include/libc/sys

# The platform Makefile contains hw details and flags
include target/$(TARGET)/Makefile

CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AS = $(CROSS)as
OCPY = $(CROSS)objcopy
HOSTCC=gcc

# warning: return type of 'main' is not 'int' [-Wmain]
CFLAGS += \
	-mcpu=$(CPU) -mthumb -Iinclude -Iinclude/libc -I. -Icmsis/arm \
	-Iinclude/kernel \
	-Wno-main -DCONFIG_KERNEL_STACK_CHECKING -fdiagnostics-color

# ld must know the architecture because we use the stdlib (printf, memcpy..)
LDFLAGS = -mthumb -march=$(ARCH) -nostartfiles -Wl,-Map=$(NAME).map -Wl,-Tvega.lds

#FIXME: revisit the arch-specific imports
ifeq ($(ARCH),armv6-m)
	SSRC += v6m-entry.S
	CSRC += v6m-faults.c
else
	SSRC += arch/v7m-head.S arch/v7m-entry.S arch/v7m-svcall.S \
		libc/v7m-pthread.S
	CSRC += arch/v7m-faults.c
endif

SSRC += $(wildcard libc/vega/*.S)
CSRC += $(wildcard libc/*.c)		\
	$(wildcard kernel/*.c)		\
	$(wildcard kernel/fs/*.c)	\
	$(wildcard drivers/char/*.c)	\
	$(wildcard drivers/mtd/*.c)	\
	$(wildcard drivers/timer/timercore.c) \
	$(wildcard drivers/serial/serial*.c) \
	$(wildcard system/*.c)		\

OBJS += $(SSRC:.S=.o) $(CSRC:.c=.o)
OBJS := $(sort $(OBJS))

all: include/version.h $(NAME).lds $(NAME).hex

$(NAME).elf: $(OBJS)
	$(VECHO) "LD\t$@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(VECHO) "CC\t$@"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -W -Wall -std=c11 -D__KERNEL__ $<

%.o: %.S
	$(VECHO) "AS\t$@"
	$(Q)$(CC) -o $@ $(CFLAGS) -c $<

%.lds: %.lds.S
	$(VECHO) "HOSTCC\t$@"
	$(Q)$(HOSTCC) -E -P -Iinclude -D__LINKER__ -DROMSZ=$(ROMSZ) -DRAMSZ=$(RAMSZ) -o $@ $<

include/version.h: include/version.template.h
	$(VECHO) "GEN\t$@"
	$(Q)cat $< | sed -e "s/GIT_COMMIT/`git log --pretty=format:'%h' -n 1`/g" \
	-e "s/GIT_BRANCH/`git symbolic-ref --short HEAD`/g" > $@

%.hex: %.elf
	$(VECHO) "OBJCOPY\t$@"
	$(Q)$(OCPY) -O ihex $< $@

EMACS_TRASH = $(foreach dir,$(DIRS),$(wildcard $(dir)/*~))

clean::
	rm -f $(OBJS) $(NAME).map $(NAME).lds include/version.h $(EMACS_TRASH)

distclean: clean
	rm -f $(NAME).elf $(NAME).hex

# platform Makefile.rules contains flashing and running rules
include target/$(TARGET)/Makefile.rules
