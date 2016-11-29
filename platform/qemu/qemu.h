/*
 * platform/qemu/qemu.h
 *
 * Copyright (C) 2016 Benoit Marcot
 */

#ifndef _PLATFORM_QEMU_QEMU_H
#define _PLATFORM_QEMU_QEMU_H

/* qemu-2.5.0 supports semihosting */
void semih_exit(int status);

#endif /* !_PLATFORM_QEMU_QEMU_H */
