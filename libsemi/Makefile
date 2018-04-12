SRC	= $(wildcard *.S)
OBJ	= $(SRC:.S=.o)
CROSS	= arm-none-eabi-
CC	= $(CROSS)gcc

.PHONY: all clean

all: libsemi.a

libsemi.a: $(OBJ)
	$(AR) rcs $@ $^

%.o: %.S
	$(CC) -o $@ -march=armv7-m -W -Wall -c $<

clean:
	rm -f libsemi.a *.o
