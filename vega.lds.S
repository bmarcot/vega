#include "utils.h"

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(cold_reset)

MEMORY
{
    rom (rx)  : ORIGIN = 0, LENGTH = ROMSZ
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = RAMSZ
}

SECTIONS
{
    .vector.early : {
    	PROVIDE(vtor_early_base = .);
    	*(.vector.early)
    } > rom

    .vector ALIGN(128) : {
    	PROVIDE(vector_base = .);
    	*(.vector)
    } > rom

    .text ALIGN(8) : {
        *(.text)
	*(.text.*)
	*(.rodata)
	*(.rodata.*)
	PROVIDE(__text_end__ = .);
    } > rom

    /* Initialized data (.data* sections) are initially stored in Flash,
       but need to be copied to a volatile storage for they are not
       read-only.    */

    .data /* ALIGN(32) */ : AT (__text_end__) {
    	. = ALIGN(32);
    	PROVIDE(__data_start__ = .);
        *(.data)
        *(.data.*)
    	. = ALIGN(32);
    } > ram

    PROVIDE(__data_size__ = SIZEOF(.data));

    .bss : {
    	PROVIDE(__bss_start__ = .);
        *(.bss)
        *(COMMON)
	. = __bss_start__ + align_hi(. - __bss_start__, 32);
    } > ram

    PROVIDE(__bss_size__ = SIZEOF(.bss));

    .heap ALIGN(32) : {
        PROVIDE(heap_base = .);
        . += 4k;
        PROVIDE(heap_limit = .);
    } > ram

    .early.stack ALIGN(8) : {
        PROVIDE(early_stack_limit = .);
        . += 1k;
        PROVIDE(early_stack_base = .);
    } > ram

    .pgmem ALIGN(2k) : {
        PROVIDE(__pgmem_base__ = .);
        . += 32k;
    } > ram

    PROVIDE(__pgmem_size__ = SIZEOF(.pgmem));
}