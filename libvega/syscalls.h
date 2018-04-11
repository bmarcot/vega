/* Copyright (c) 2018 Benoit Marcot */

#ifndef LIBVEGA_SYSCALLS_H
#define LIBVEGA_SYSCALLS_H

long _syscall0();
long _syscall1();
long _syscall2();
long _syscall3();
long _syscall4();
long _syscall5();
long _syscall6();

#define syscall(argc, ...) _syscall##argc(__VA_ARGS__)

#endif /* !LIBVEGA_SYSCALLS_H */
