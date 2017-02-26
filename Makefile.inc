#
# Makefile.inc
#
# Copyright (c) 2017 Benoit Marcot
#

CROSS   = arm-none-eabi-
CC      = $(CROSS)gcc
AS      = $(CROSS)as
AR      = $(CROSS)ar
OBJCOPY = $(CROSS)objcopy
HOSTCC  = gcc

CFLAGS  += -mthumb -mcpu=$(CPU)
LDFLAGS += -mthumb -march=$(ARCH)
