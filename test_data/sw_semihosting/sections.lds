
MEMORY
{
    ram (ax)  : ORIGIN = 0x00000000, LENGTH = 4K
}

SECTIONS 
{
    .text : 
    {
        start.o(.text*)
        *(.text*)
    }  > ram

    .rodata ALIGN(4) : 
    {
        *(.rodata*)
        *(.srodata*)
    }  > ram

    .data ALIGN (8) :
    {
        *(.data*)
        *(.sdata*)
    } > ram

    .bss ALIGN(8) (NOLOAD) :
    {
        *(.bss*)
        *(.sbss*)
        *(COMMON)
    } > ram

	_end = .;
}
