SRC	= $(wildcard *.S)
OBJ	= $(SRC:.S=.o)
CROSS	= arm-none-eabi-
CC	= $(CROSS)gcc

.PHONY: all clean

all: libvega.a

libvega.a: $(OBJ)
	$(AR) rcs $@ $^

%.o: %.S
	$(CC) -o $@ -march=armv7-m -W -Wall -c $<

clean:
	rm -f libvega.a *.o
