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
	PROVIDE(__text_start__ = .);
        *(.text)
	*(.text.*)
	PROVIDE(__text_end__ = .);
	PROVIDE(__rodata_start__ = .);
	*(.rodata)
	*(.rodata.*)
	PROVIDE(__rodata_end__ = .);
    } > rom

    /* Initialized data (.data* sections) are initially stored in Flash,
       but need to be copied to a volatile storage for they are not
       read-only.    */

    .data : AT (__rodata_end__) {
    	PROVIDE(__data_start__ = .);
        *(.data)
        *(.data.*)
	PROVIDE(__data_end__ = .);
    } > ram

    PROVIDE(__data_size__ = SIZEOF(.data));

    .bss : {
    	PROVIDE(__bss_start__ = .);
        *(.bss)
        *(COMMON)
	PROVIDE(__bss_end__ = .);
    } > ram

    PROVIDE(__bss_size__ = SIZEOF(.bss));

    .heap ALIGN(32) : {
        PROVIDE(heap_base = .);
        . += 4k;
        PROVIDE(heap_limit = .);
    } > ram

    .pgmem 0x20000000 + RAMSZ - 32k : {
	PROVIDE(__pgmem_start__ = .);
	. += 30k;

	/* Reserve the last page for the early stack. Must be of the biggest size
	 * such that there will be no coalescing when freeing the page, for lower
	 * booting time.    */
	PROVIDE(__early_stack_end__ = .);
	. += 2k;
	PROVIDE(__early_stack_start__ = .);

	PROVIDE(__pgmem_end__ = .);
    } > ram

    PROVIDE(__pgmem_size__ = SIZEOF(.pgmem));
}
