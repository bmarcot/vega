SRC	= $(wildcard *.c)
CROSS   = arm-none-eabi-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
PYTHON	= python3

all: ../initrd.o

init.d/init: $(SRC)
	mkdir -p init.d; $(CC) -o $@ -mcpu=cortex-m3 -mthumb -pie -Wl,-z,max-page-size=0 -nostdlib -nostartfiles $^

initrd.img:
	$(PYTHON) geninitfs.py --dir init.d --output $@ --bfdarch arm --verbose

../initrd.o: initrd.img
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata $^ $@

clean:
	rm -f init.d/init ../initrd.o initrd.img
