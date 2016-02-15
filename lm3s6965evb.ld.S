#include "utils.h"

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(cold_reset)

MEMORY
{
    rom (rx)  : ORIGIN = 0, LENGTH = 256k
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 64k
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

    /* 
     * This vector is used as a trampoline to Handler_Mode
     * and lives only for a few instructions. It should not
     * allocate/reserve any physical memory for that purpose.
     * Instead overlay that section and another, or reclaim
     * that memory after kernel is started.
     */
    .vector.trampoline ALIGN(128) : {
        PROVIDE(vtor_trampoline_base = .);
        . += 64;
    } > ram

    .heap ALIGN(32) : {
        PROVIDE(heap_base = .);
        . += 4k;
        PROVIDE(heap_limit = .);
    } > ram

    /* same as .vector.trampoline */
    .early.stack ALIGN(8) : {
        PROVIDE(early_stack_limit = .);
        . += 1k;
        PROVIDE(early_stack_base = .);
    } > ram

    .pgpool ALIGN(1k) : {
        PROVIDE(pgpool_base = .);
        . += 32k;
        PROVIDE(pgpool_limit = .);
    } > ram
}
