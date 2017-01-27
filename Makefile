
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
	SSRC += arch/v7m-head.S arch/v7m-entry.S arch/v7m-svcall.S
	CSRC += arch/v7m-faults.c
endif

LIBVEGA_CSRC = $(wildcard libc/*.c)
LIBVEGA_SSRC = $(wildcard libc/*.S) $(wildcard libc/vega/*.S)

LIBVEGA_OBJS = $(LIBVEGA_SSRC:.S=.o) $(LIBVEGA_CSRC:.c=.o)

CSRC += $(wildcard kernel/*.c)		\
	$(wildcard kernel/fs/*.c)	\
	$(wildcard drivers/char/*.c)	\
	$(wildcard drivers/mtd/*.c)	\
	$(wildcard drivers/timer/timercore.c) \
	$(wildcard drivers/serial/serial*.c) \
	$(wildcard system/*.c)		\

OBJS += $(SSRC:.S=.o) $(CSRC:.c=.o)
OBJS := $(sort $(OBJS))

all: include/version.h $(NAME).lds $(NAME).hex

$(NAME).elf: $(OBJS) libvega.a
	$(VECHO) "LD\t$@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

libvega.a: $(LIBVEGA_OBJS)
	$(VECHO) "AR\t$@"
	$(Q)$(AR) rcs $@ $^

%.o: %.c
	$(VECHO) "CC\t$@"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -W -Wall -std=c11 -D__KERNEL__ $<

%.o: %.S
	$(VECHO) "AS\t$@"
	$(Q)$(CC) -o $@ $(CFLAGS) -c $<

%.lds: %.lds.S
	$(VECHO) "HOSTCC\t$@"
	$(Q)$(HOSTCC) -E -P -Iinclude -DROMSZ=$(ROMSZ) -DRAMSZ=$(RAMSZ) -o $@ $<

include/version.h: include/version.template.h
	$(VECHO) "GEN\t$@"
	$(Q)cat $< | sed -e "s/GIT_COMMIT/`git log --pretty=format:'%h' -n 1`/g" \
	-e "s/GIT_BRANCH/`git symbolic-ref --short HEAD`/g" > $@

%.hex: %.elf
	$(VECHO) "OBJCOPY\t$@"
	$(Q)$(OCPY) -O ihex $< $@

DIRS =	arch		\
	drivers		\
	kernel		\
	platform	\
	system		\
	target

clean::
	find $(DIRS) -name "*.o" -type f -delete
	rm -f $(NAME).map $(NAME).lds include/version.h

clean_cmsis:
	find libc -name "*.o" -type f -delete

clean_libvega:
	find libc -name "*.o" -type f -delete
	rm -f libvega.a

distclean: clean clean_libvega clean_cmsis
	rm -f $(NAME).elf $(NAME).hex
	find . -name "*~" -type f -delete

# platform Makefile.rules contains flashing and running rules
include target/$(TARGET)/Makefile.rules
