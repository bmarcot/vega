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
    	*(.vector.early)
    } > rom

    .vector ALIGN(128) : {
    	*(.vector)
    } > rom

    .text ALIGN(8) : {
        *(.text)
	*(.text.*)
	*(.rodata)
	*(.rodata.*)
    } > rom

    .data : {
        *(.data)
        *(.data.*)
    } > ram

    .bss : {
        *(.bss)
        *(COMMON)
    } > ram
}
