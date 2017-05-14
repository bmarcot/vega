/*
 * include/uapi/elf32-types.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _UAPI_ELF32_TYPES_H
#define _UAPI_ELF32_TYPES_H

#include <stdint.h>

/* 32-bit ELF base types. */
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

#endif /* !_UAPI_ELF32_TYPES_H */
