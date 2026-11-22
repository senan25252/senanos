OUTPUT_FORMAT(pei-x86-64)
ENTRY(kernel_main)

SECTIONS
{
    . = 0x9000;
    .text : { KEEP(*(.text.kernel_entry)) *(.text*) }
    .rdata : { *(.rdata*) *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) *(COMMON) }
    /DISCARD/ : { *(.eh_frame*) *(.pdata*) *(.xdata*) *(.drectve*) *(.comment*) }
}
