#
# Makefile
#
# Copyright (c) 2015-2017 Benoit Marcot
#

include Makefile.opt
include Makefile.inc

# vegaz, compressed kernel
NAME = vega

# Build and run on Qemu when the target is unspecified
TARGET ?= qemu

# The platform Makefile contains hw details and flags
include target/$(TARGET)/Makefile

# warning: return type of 'main' is not 'int' [-Wmain]
CFLAGS += \
	-DCONFIG_KERNEL_STACK_CHECKING		\
	-DCONFIG_THREAD_INFO_IN_TASK		\
	-DHRTIMER_DEVICE='"$(HRTIMER_DEVICE)"'	\
	-Wno-main				\
	-Wno-unused-parameter			\
	-Wno-unused-function			\
	-fdiagnostics-color			\
	-ffunction-sections -fdata-sections -Os \
	-I.					\
	-Iinclude				\
	-Iinclude/libc				\
	-Icmsis/arm				\
	-Ilibc/include				\
	-Iarch/arm/include			\
	-Iarch/arm/include/uapi			\

LDFLAGS += \
	-nostartfiles		\
	-specs=nano.specs	\
	-Wl,-Map=$(NAME).map	\
	-Wl,-Tvega.lds		\
	-Wl,--gc-sections

ifeq ($(ARCH),armv6-m)
	SSRC += v6m-entry.S
	CSRC += v6m-faults.c
else
	SSRC += $(wildcard arch/arm/kernel/*.S)
	CSRC += $(wildcard arch/arm/kernel/*.c)
endif

LIBVEGA_CSRC = $(wildcard libc/*.c)
LIBVEGA_SSRC = $(wildcard libc/*.S) $(wildcard libc/vega/*.S)

SSRC += $(LIBVEGA_SSRC)

CSRC += $(wildcard kernel/*.c)		\
	$(wildcard kernel/fs/*.c)	\
	$(wildcard kernel/mm/*.c)	\
	$(wildcard kernel/time/*.c)	\
	$(wildcard drivers/char/*.c)	\
	$(wildcard drivers/mtd/*.c)	\
	$(wildcard drivers/serial/serial*.c) \
	$(wildcard system/*.c)		\
	libc/vega/stubs.c		\
	libc/vega/mman.c		\
	$(LIBVEGA_CSRC)			\

OBJS += $(SSRC:.S=.o) $(CSRC:.c=.o)
OBJS := $(sort $(OBJS))

.PHONY: all clean distclean

all: $(NAME).lds $(NAME).hex

$(NAME).elf: $(OBJS) kernel/fs/version.o libvega/libvega.a libsemi/libsemi.a initrd.o
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
	$(Q)$(HOSTCC) -E -P -Iinclude -DROMSZ=$(ROMSZ) -DRAMSZ=$(RAMSZ) -o $@ $<

kernel/fs/version:
	$(VECHO) "GEN\t$@"
	$(Q)python3 scripts/gen-proc-version.py --cc-version --git-branch	\
	--git-commit --user $(shell whoami) --host $(shell hostname)		\
	-a $(ARCH) -c $(CPU) -n 'Vega' > $@

kernel/fs/version.o: kernel/fs/version
	$(VECHO) "OBJCOPY\t$@"
	$(Q)$(OBJCOPY) -I binary -O elf32-littlearm -B arm		\
	--rename-section .data=.rodata					\
        --redefine-sym _binary_$(subst /,_,$<)_start=_version_ptr	\
        --redefine-sym _binary_$(subst /,_,$<)_size=_version_len	\
	$< $@

libvega/libvega.a:
	$(MAKE) -C libvega

libsemi/libsemi.a:
	$(MAKE) -C libsemi

initrd.o:
	$(MAKE) -C init

%.hex: %.elf
	$(VECHO) "OBJCOPY\t$@"
	$(Q)$(OBJCOPY) -O ihex $< $@

clean::
	find . -name "*.o" -type f -delete
	rm -f $(NAME).map $(NAME).lds
	rm -f kernel/fs/version
	$(MAKE) -C init clean
	$(MAKE) -C libvega clean
	$(MAKE) -C libsemi clean

distclean: clean
	rm -f $(NAME).elf $(NAME).hex
	find . -name "*~" -type f -delete

# platform Makefile.rules contains flashing and running rules
include target/$(TARGET)/Makefile.rules
