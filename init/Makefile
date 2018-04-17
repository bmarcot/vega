SRC	= $(wildcard *.c)
CROSS	= arm-none-eabi-
CC	= $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
STRIP	= $(CROSS)strip
PYTHON	= python3

.PHONY: all clean

all: ../initrd.o

init.d/init: $(SRC)
	$(CC) -o $@ -mcpu=cortex-m3 -mthumb -pie -Wl,-z,max-page-size=0 -nostdlib -nostartfiles \
		-I../include -I../arch/arm/include/uapi $^ \
		-L../libsemi -L../libvega -lsemi -lvega
	$(STRIP) --remove-section=.comment $@

initrd.img: init.d/init
	$(PYTHON) geninitfs.py --dir init.d --output $@ --bfdarch arm

../initrd.o: initrd.img
	$(OBJCOPY) --input-target=binary --output-target=elf32-littlearm --binary-architecture=arm \
		--rename-section .data=.rodata $^ $@

clean:
	rm -f init.d/init ../initrd.o initrd.img
