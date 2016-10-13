
NAME	= epd

CROSS	= arm-none-eabi-
CC	= $(CROSS)gcc
AS	= $(CROSS)as
OCPY	= $(CROSS)objcopy
HOSTCC	= gcc

# warning: return type of 'main' is not 'int' [-Wmain]
CFLAGS	+= -mcpu=cortex-m4 -mthumb -I. -IPervasive_Displays_small_EPD

# ld must know the architecture because we use the stdlib (printf, memcpy..)
LDFLAGS	= -mthumb -march=armv7e-m -nostartfiles

CSRC	+= $(wildcard Pervasive_Displays_small_EPD/*.c)
CSRC	+= Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb/EPD_G2_Aurora_Mb.c
OBJS	+= $(CSRC:.c=.o)

all: $(NAME).elf

$(NAME).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -std=c11 $<

clean::
	rm -f $(OBJS) $(NAME).map $(NAME).lds include/version.h $(EMACS_TRASH)

distclean: clean
	rm -f $(NAME).elf $(NAME).hex
