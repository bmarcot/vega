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
    } > rom

    .data : {
        *(.data)
        *(.data.*)
    } > ram

    .bss : {
        *(.bss)
        *(COMMON)
    } > ram

    /* 
     * This vector is used as a trampoline to Handler_Mode
     * and lives only for a few instructions. It should not
     * allocate any physical memory for that purpose.
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

    .pgpool ALIGN(1k) : {
        PROVIDE(pgpool_base = .);
        . += 32k;
        PROVIDE(pgpool_limit = .);
    } > ram
}
